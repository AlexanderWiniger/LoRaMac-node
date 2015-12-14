/*
 / _____)             _              | |
 ( (____  _____ ____ _| |_ _____  ____| |__
 \____ \| ___ |    (_   _) ___ |/ ___)  _ \
 _____) ) ____| | | || |_| ____( (___| | | |
 (______/|_____)_|_|_| \__)_____)\____)_| |_|
 (C)2013 Semtech

 Description: Timer objects and scheduling management

 License: Revised BSD License, see LICENSE.TXT file include in the project

 Maintainer: Miguel Luis and Gregory Cristian
 */
#include "board.h"
#include "rtc-board.h"
#include "timer-board.h"

#define LOG_LEVEL_TRACE
#include "debug.h"

static bool LowPowerModeEnable = true;

#if defined(FSL_RTOS_FREE_RTOS) || defined(USE_FREE_RTOS)
static uint32_t NrOfTimers = 0;

void TimerInit( TimerEvent_t *obj, const char* name, uint32_t id, uint32_t periodInMs, TimerCallbackFunction_t callback, bool autoReload )
{
    if(NrOfTimers > configTIMER_QUEUE_LENGTH) {
        LOG_ERROR("Maximum number of timers created.");
    }

    obj->Handle = xTimerCreate( name,
            (periodInMs / portTICK_PERIOD_MS),
            autoReload,
            ( void * ) (id),
            callback
    );

    if(obj->Handle != NULL) {
        obj->PeriodInMs = periodInMs;
        obj->AutoReload = autoReload;
        obj->Callback = callback;
        obj->IsRunning = false;
        NrOfTimers++;

        LOG_TRACE("%s created.", name);
    } else {
        LOG_ERROR("Failed to create %s timer", name);
    }
}

void TimerStart( TimerEvent_t *obj )
{
    BaseType_t xReturn = pdFAIL;

    if(obj->IsRunning) return;

    if(__get_IPSR()) {
        xTimerStartFromISR(obj->Handle, &xReturn);
    } else {
        xReturn = xTimerStart( obj->Handle, 100 );
    }

    if(xReturn != pdFAIL) {
        obj->IsRunning = true;
        LOG_TRACE("%s started.", pcTimerGetTimerName(obj->Handle));
    } else {
        LOG_ERROR("Failed to start %s timer", pcTimerGetTimerName(obj->Handle));
    }
}

void TimerStop( TimerEvent_t *obj )
{
    BaseType_t xReturn = pdFAIL;

    if(!obj->IsRunning) return;

    if(__get_IPSR()) {
        xTimerStopFromISR(obj->Handle, &xReturn);
    } else {
        xReturn = xTimerStop( obj->Handle, 100 );
    }

    if(xReturn != pdFAIL) {
        obj->IsRunning = false;
        LOG_TRACE("%s stopped.", pcTimerGetTimerName(obj->Handle));
    } else {
        LOG_ERROR("Failed to stop %s timer", pcTimerGetTimerName(obj->Handle));
    }
}

void TimerReset( TimerEvent_t *obj )
{
    BaseType_t xReturn = pdFAIL;

    if(__get_IPSR()) {
        xTimerResetFromISR(obj->Handle, &xReturn);
    } else {
        xReturn = xTimerReset(obj->Handle, 100);
    }

    if(xReturn != pdFAIL) {
        obj->IsRunning = true;
        LOG_TRACE("%s reset.", pcTimerGetTimerName(obj->Handle));
    } else {
        LOG_ERROR("Failed to reset %s timer", pcTimerGetTimerName(obj->Handle));
    }
}

void TimerSetValue( TimerEvent_t *obj, uint32_t value )
{
    BaseType_t xReturn = pdFAIL;

    if(__get_IPSR()) {
        xTimerChangePeriodFromISR(obj->Handle, (value / portTICK_PERIOD_MS), &xReturn);
    } else {
        xReturn = xTimerChangePeriod(obj->Handle, (value / portTICK_PERIOD_MS), 100);
    }

    if (xReturn != pdFAIL) {
        obj->PeriodInMs = value;
        LOG_TRACE("%s period changed from %u to %u.", pcTimerGetTimerName(obj->Handle), obj->PeriodInMs, value);
    } else {
        LOG_ERROR("Failed to change %s timers value from %u to %u", pcTimerGetTimerName(obj->Handle), obj->PeriodInMs, (value / portTICK_PERIOD_MS));
    }

}

TimerTime_t TimerGetCurrentTime( void )
{
    return (TimerTime_t)xTaskGetTickCount();
}

void TimerLowPowerHandler( void )
{

}

/*!
 * \brief Manages the entry into ARM cortex deep-sleep mode
 */
void TimerLowPowerHandler( void );

#else
/*!
 * This flag is used to make sure we have looped through the main several time to avoid race issues
 */
volatile uint8_t HasLoopedThroughMain = 0;

/*!
 * Timers list head pointer
 */
static TimerEvent_t *TimerListHead = NULL;

/*!
 * \brief Adds or replace the head timer of the list.
 *
 * \remark The list is automatically sorted. The list head always contains the
 *         next timer to expire.
 *
 * \param [IN]  obj Timer object to be become the new head
 * \param [IN]  remainingTime Remaining time of the previous head to be replaced
 */
static void TimerInsertNewHeadTimer( TimerEvent_t *obj, uint32_t remainingTime );

/*!
 * \brief Adds a timer to the list.
 *
 * \remark The list is automatically sorted. The list head always contains the
 *         next timer to expire.
 *
 * \param [IN]  obj Timer object to be added to the list
 * \param [IN]  remainingTime Remaining time of the running head after which the object may be added
 */
static void TimerInsertTimer( TimerEvent_t *obj, uint32_t remainingTime );

/*!
 * \brief Sets a timeout with the duration "timestamp"
 * 
 * \param [IN] timestamp Delay duration
 */
static void TimerSetTimeout( TimerEvent_t *obj );

/*!
 * \brief Check if the Object to be added is not already in the list
 * 
 * \param [IN] timestamp Delay duration
 * \retval true (the object is already in the list) or false  
 */
static bool TimerExists( TimerEvent_t *obj );

/*!
 * \brief Read the timer value of the currently running timer
 *
 * \retval value current timer value
 */
uint32_t TimerGetValue( void );

void TimerInit( TimerEvent_t *obj, void (*callback)( void ) )
{
    obj->Timestamp = 0;
    obj->ReloadValue = 0;
    obj->IsRunning = false;
    obj->Callback = callback;
    obj->Next = NULL;
}

void TimerStart( TimerEvent_t *obj )
{
    uint32_t elapsedTime = 0;
    uint32_t remainingTime = 0;

    __disable_irq();

    if ( (obj == NULL) || (TimerExists(obj) == true) ) {
        __enable_irq();
        return;
    }

    obj->Timestamp = obj->ReloadValue;
    obj->IsRunning = false;

    if ( TimerListHead == NULL ) {
        TimerInsertNewHeadTimer(obj, obj->Timestamp);
    } else {
        if ( TimerListHead->IsRunning == true ) {
            elapsedTime = TimerGetValue();
            if ( elapsedTime > TimerListHead->Timestamp ) {
                elapsedTime = TimerListHead->Timestamp; // security but should never occur
            }
            remainingTime = TimerListHead->Timestamp - elapsedTime;
        } else {
            remainingTime = TimerListHead->Timestamp;
        }

        if ( obj->Timestamp < remainingTime ) {
            TimerInsertNewHeadTimer(obj, remainingTime);
        } else {
            TimerInsertTimer(obj, remainingTime);
        }
    }
    __enable_irq();
}

static void TimerInsertTimer( TimerEvent_t *obj, uint32_t remainingTime )
{
    uint32_t aggregatedTimestamp = 0;      // hold the sum of timestamps 
    uint32_t aggregatedTimestampNext = 0; // hold the sum of timestamps up to the next event

    TimerEvent_t* prev = TimerListHead;
    TimerEvent_t* cur = TimerListHead->Next;

    if ( cur == NULL ) {   // obj comes just after the head
        obj->Timestamp -= remainingTime;
        prev->Next = obj;
        obj->Next = NULL;
    } else {
        aggregatedTimestamp = remainingTime;
        aggregatedTimestampNext = remainingTime + cur->Timestamp;

        while (prev != NULL) {
            if ( aggregatedTimestampNext > obj->Timestamp ) {
                obj->Timestamp -= aggregatedTimestamp;
                if ( cur != NULL ) {
                    cur->Timestamp -= obj->Timestamp;
                }
                prev->Next = obj;
                obj->Next = cur;
                break;
            } else {
                prev = cur;
                cur = cur->Next;
                if ( cur == NULL ) {   // obj comes at the end of the list
                    aggregatedTimestamp = aggregatedTimestampNext;
                    obj->Timestamp -= aggregatedTimestamp;
                    prev->Next = obj;
                    obj->Next = NULL;
                    break;
                } else {
                    aggregatedTimestamp = aggregatedTimestampNext;
                    aggregatedTimestampNext = aggregatedTimestampNext + cur->Timestamp;
                }
            }
        }
    }
}

static void TimerInsertNewHeadTimer( TimerEvent_t *obj, uint32_t remainingTime )
{
    TimerEvent_t* cur = TimerListHead;

    if ( cur != NULL ) {
        cur->Timestamp = remainingTime - obj->Timestamp;
        cur->IsRunning = false;
    }

    obj->Next = cur;
    obj->IsRunning = true;
    TimerListHead = obj;
    TimerSetTimeout(TimerListHead);
}

void TimerIrqHandler( void )
{
    uint32_t elapsedTime = 0;

    if ( LowPowerModeEnable == false ) {
        if ( TimerListHead == NULL ) {
            return; // Only necessary when the standard timer is used as a time base
        }
    }

    elapsedTime = TimerGetValue();

    TimerEvent_t* elapsedTimer = NULL;

    if ( elapsedTime > TimerListHead->Timestamp ) {
        TimerListHead->Timestamp = 0;
    } else {
        TimerListHead->Timestamp -= elapsedTime;
    }

    // save TimerListHead
    elapsedTimer = TimerListHead;

    // remove all the expired object from the list
    while ((TimerListHead != NULL) && (TimerListHead->Timestamp == 0)) {
        if ( TimerListHead->Next != NULL ) {
            TimerListHead = TimerListHead->Next;
        } else {
            TimerListHead = NULL;
        }
    }

    // execute the callbacks of all the expired objects
    // this is to avoid potential issues between the callback and the object list
    while ((elapsedTimer != NULL) && (elapsedTimer->Timestamp == 0)) {
        if ( elapsedTimer->Callback != NULL ) {
            elapsedTimer->Callback();
        }
        elapsedTimer = elapsedTimer->Next;
    }

    // start the next TimerListHead if it exists
    if ( TimerListHead != NULL ) {
        TimerListHead->IsRunning = true;
        TimerSetTimeout(TimerListHead);
    }
}

void TimerStop( TimerEvent_t *obj )
{
    __disable_irq();

    uint32_t elapsedTime = 0;
    uint32_t remainingTime = 0;

    TimerEvent_t* prev = TimerListHead;
    TimerEvent_t* cur = TimerListHead;

    // List is empty or the Obj to stop does not exist 
    if ( (TimerListHead == NULL) || (obj == NULL) ) {
        __enable_irq();
        return;
    }

    if ( TimerListHead == obj ) // Stop the Head                                    
            {
        if ( TimerListHead->IsRunning == true )  // The head is already running 
                {
            elapsedTime = TimerGetValue();
            if ( elapsedTime > obj->Timestamp ) {
                elapsedTime = obj->Timestamp;
            }

            remainingTime = obj->Timestamp - elapsedTime;

            if ( TimerListHead->Next != NULL ) {
                TimerListHead->IsRunning = false;
                TimerListHead = TimerListHead->Next;
                TimerListHead->Timestamp += remainingTime;
                TimerListHead->IsRunning = true;
                TimerSetTimeout(TimerListHead);
            } else {
                TimerListHead = NULL;
            }
        } else   // Stop the head before it is started
        {
            if ( TimerListHead->Next != NULL ) {
                remainingTime = obj->Timestamp;
                TimerListHead = TimerListHead->Next;
                TimerListHead->Timestamp += remainingTime;
            } else {
                TimerListHead = NULL;
            }
        }
    } else   // Stop an object within the list
    {
        remainingTime = obj->Timestamp;

        while (cur != NULL) {
            if ( cur == obj ) {
                if ( cur->Next != NULL ) {
                    cur = cur->Next;
                    prev->Next = cur;
                    cur->Timestamp += remainingTime;
                } else {
                    cur = NULL;
                    prev->Next = cur;
                }
                break;
            } else {
                prev = cur;
                cur = cur->Next;
            }
        }
    }
    __enable_irq();
}

static bool TimerExists( TimerEvent_t *obj )
{
    TimerEvent_t* cur = TimerListHead;

    while (cur != NULL) {
        if ( cur == obj ) {
            return true;
        }
        cur = cur->Next;
    }
    return false;
}

void TimerReset( TimerEvent_t *obj )
{
    TimerStop(obj);
    TimerStart(obj);
}

void TimerSetValue( TimerEvent_t *obj, uint32_t value )
{
    uint32_t minValue = 0;

    TimerStop(obj);

    if ( LowPowerModeEnable == true ) {
        minValue = RtcGetMinimumTimeout();
    } else {
        minValue = TimerHwGetMinimumTimeout();
    }

    if ( value < minValue ) {
        value = minValue;
    }

    obj->Timestamp = value;
    obj->ReloadValue = value;
}

uint32_t TimerGetValue( void )
{
    if ( LowPowerModeEnable == true ) {
        return RtcGetTimerElapsedTime();
    } else {
        return TimerHwGetElapsedTime();
    }
}

TimerTime_t TimerGetCurrentTime( void )
{
    if ( LowPowerModeEnable == true ) {
        return RtcGetTimerValue();
    } else {
        return TimerHwGetTime();
    }
}

static void TimerSetTimeout( TimerEvent_t *obj )
{
    HasLoopedThroughMain = 0;

    if ( LowPowerModeEnable == true ) {
        RtcSetTimeout(obj->Timestamp);
    } else {
        TimerHwStart(obj->Timestamp);
    }
}

void TimerLowPowerHandler( void )
{
    if ( (TimerListHead != NULL) && (TimerListHead->IsRunning == true) ) {
        if ( HasLoopedThroughMain < 5 ) {
            HasLoopedThroughMain++;
        } else {
            HasLoopedThroughMain = 0;

            if ( LowPowerModeEnable == true ) {
                RtcEnterLowPowerStopMode();
            } else {
                TimerHwEnterLowPowerStopMode();
            }
        }
    }
}
#endif

void TimerSetLowPowerEnable( bool enable )
{
    LowPowerModeEnable = enable;
}

bool TimerGetLowPowerEnable( void )
{
    return LowPowerModeEnable;
}
