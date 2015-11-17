/**
 * \file main.c
 * \author Alexander Winiger (alexander.winiger@hslu.ch)
 * \date 12.11.2015
 * \brief LoRaMesh implementation
 *
 */
#include <string.h>
#include <math.h>
#include "board.h"

#include "LoRaMesh.h"

#define LOG_LEVEL_TRACE
#include "debug.h"

/*!
 * When set to 1 the application uses the Over-the-Air activation procedure
 * When set to 0 the application uses the Personalization activation procedure
 */
#define OVER_THE_AIR_ACTIVATION                     0

/*!
 * Indicates if the end-device is to be connected to a private or public network
 */
#define LORAWAN_PUBLIC_NETWORK                      true

#if( OVER_THE_AIR_ACTIVATION != 0 )

/*!
 * Join requests trials duty cycle.
 */
#define OVER_THE_AIR_ACTIVATION_DUTYCYCLE           10000000  // 10 [s] value in us

/*!
 * Mote device IEEE EUI
 *
 * \remark must be written as a little endian value (reverse order of normal reading)
 *
 * \remark In this application the value is automatically generated by calling
 *         BoardGetUniqueId function
 */
#define LORAWAN_DEVICE_EUI                          { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }

/*!
 * Application IEEE EUI
 *
 * \remark must be written as a little endian value (reverse order of normal reading)
 */
#define LORAWAN_APPLICATION_EUI                     { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }

/*!
 * AES encryption/decryption cipher application key
 */
#define LORAWAN_APPLICATION_KEY                     { 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF, 0x00 }

#else

/*!
 * Current network ID
 */
#define LORAWAN_NETWORK_ID                          ( uint32_t )0

/*!
 * Device address on the network
 *
 * \remark must be written as a big endian value (normal reading order)
 *
 * \remark In this application the value is automatically generated using
 *         a pseudo random generator seeded with a value derived from
 *         BoardUniqueId value
 */
#define LORAWAN_DEVICE_ADDRESS                      ( uint32_t )0x00000000

/*!
 * AES encryption/decryption cipher network session key
 */
#define LORAWAN_NWKSKEY                             { 0x2B, 0x7E, 0x15, 0x16, 0x28, 0xAE, 0xD2, 0xA6, 0xAB, 0xF7, 0x15, 0x88, 0x09, 0xCF, 0x4F, 0x3C }

/*!
 * AES encryption/decryption cipher application session key
 */
#define LORAWAN_APPSKEY                             { 0x2B, 0x7E, 0x15, 0x16, 0x28, 0xAE, 0xD2, 0xA6, 0xAB, 0xF7, 0x15, 0x88, 0x09, 0xCF, 0x4F, 0x3C }

#endif

/*!
 * Defines the application data transmission duty cycle
 */
#define APP_TX_DUTYCYCLE                            1000000  // 5 [s] value in us
#define APP_TX_DUTYCYCLE_RND                        1000000  // 1 [s] value in us

/*!
 *  Pilot data header option list.
 */
#define SDUHDR_OPTION_LIST_MASK                     0x0F
#define SDUHDR_OPTION_LIST_ALT_GPS_MASK             0x08
#define SDUHDR_OPTION_LIST_ALT_BAR_MASK             0x04
#define SDUHDR_OPTION_LIST_VEC_TRACK_MASK           0x02
#define SDUHDR_OPTION_LIST_WIND_SPEED_MASK          0x01

/*!
 * LoRaWAN confirmed messages
 */
#define LORAWAN_CONFIRMED_MSG_ON                    false

/*!
 * LoRaWAN Adaptative Data Rate
 *
 * \remark Please note that when ADR is enabled the end-device should be static
 */
#define LORAWAN_ADR_ON                              false

/*!
 * LoRaWAN ETSI duty cycle control enable/disable
 *
 * \remark Please note that ETSI mandates duty cycled transmissions. Use only for test purposes
 */
#define LORAWAN_DUTYCYCLE_ON                        false

/*!
 * LoRaWAN application port
 */
#define LORAWAN_APP_PORT                            2

/*!
 * User application data buffer size
 */
#define LORAWAN_APP_DATA_SIZE                       23

#if( OVER_THE_AIR_ACTIVATION != 0 )

static uint8_t DevEui[] = LORAWAN_DEVICE_EUI;
static uint8_t AppEui[] = LORAWAN_APPLICATION_EUI;
static uint8_t AppKey[] = LORAWAN_APPLICATION_KEY;

#else

static uint8_t NwkSKey[] = LORAWAN_NWKSKEY;
static uint8_t AppSKey[] = LORAWAN_APPSKEY;

/*!
 * Device address
 */
static uint32_t DevAddr;

#endif

/*!
 * Indicates if the MAC layer has already joined a network.
 */
static bool IsNetworkJoined = false;

/*!
 * Application port
 */
static uint8_t AppPort = LORAWAN_APP_PORT;

/*!
 * User application data size
 */
static uint8_t AppDataSize = LORAWAN_APP_DATA_SIZE;

/*!
 * User application data buffer size
 */
#define LORAWAN_APP_DATA_MAX_SIZE                           64

/*!
 * User application data
 */
static uint8_t AppData[LORAWAN_APP_DATA_MAX_SIZE];

/*!
 * Indicates if the node is sending confirmed or unconfirmed messages
 */
static uint8_t IsTxConfirmed = LORAWAN_CONFIRMED_MSG_ON;

/*!
 * Defines the application data transmission duty cycle
 */
static uint32_t TxDutyCycleTime;

static TimerEvent_t TxNextPacketTimer;

#if( OVER_THE_AIR_ACTIVATION != 0 )

/*!
 * Defines the join request timer
 */
static TimerEvent_t JoinReqTimer;

#endif

/*!
 * Indicates if a new packet can be sent
 */
static bool TxNextPacket = true;
static bool ScheduleNextTx = false;

static LoRaMacCallbacks_t LoRaMacCallbacks;

/*!
 * LoRaMac altitude data structure.
 */
typedef struct Altitude_s {
    uint16_t GPS;
    uint16_t Barometric;
} Altitude_t;

/*!
 * LoRaMac track data structure.
 */
typedef struct VectorTrack_s {
    uint16_t GroundSpeed;
    uint16_t Track;
} VectorTrack_t;

/*!
 * LoRaMac pilot data structure.
 */
typedef struct PilotData_s {
    uint32_t Time;
    Position_t Position;
    Altitude_t Altitude;
    VectorTrack_t VectorTrack;
    uint16_t WindSpeed;
} PilotData_t;

/*!
 * Prepares the frame buffer to be sent
 */
static void PrepareTxFrame(uint8_t port)
{
    switch (port) {
        case 2:
        {
            AppData[0] = 0x0F; /* SDUHDR */
            AppData[1] = 0x56; /* Time byte 3 */
            AppData[2] = 0x46; /* Time byte 2 */
            AppData[3] = 0x21; /* Time byte 1 */
            AppData[4] = 0xD9; /* Time byte 0 */
            AppData[5] = 0x2B; /* Latitude byte 3 */
            AppData[6] = 0x61; /* Latitude byte 2 */
            AppData[7] = 0x75; /* Latitude byte 1 */
            AppData[8] = 0xFA; /* Latitude byte 0 */
            AppData[9] = 0x74; /* Longitude byte 3 */
            AppData[10] = 0x24; /* Longitude byte 2 */
            AppData[11] = 0xD3; /* Longitude byte 1 */
            AppData[12] = 0xC9; /* Longitude byte 0 */
            AppData[13] = 0x03; /* Altitude (GPS) byte 1 */
            AppData[14] = 0xB9; /* Altitude (GPS) byte 0 */
            AppData[15] = 0x03; /* Altitude (barometric) byte 1 */
            AppData[16] = 0xD3; /* Altitude (barometric) byte 0 */
            AppData[17] = 0x00; /* GroundSpeed byte 1 */
            AppData[18] = 0x2A; /* GroundSpeed byte 0 */
            AppData[19] = 0x01; /* VectorTrack byte 1 */
            AppData[20] = 0x41; /* VectorTrack byte 0 */
            AppData[21] = 0x00; /* WindSpeed byte 1 */
            AppData[22] = 0xA7; /* WindSpeed byte 0 */
        }
            break;
        default:
            break;
    }
}

static void ProcessRxFrame(LoRaMacEventFlags_t *flags, LoRaMacEventInfo_t *info)
{
    switch (info->RxPort) // Check Rx port number
    {
        case 2:
            if (info->RxBufferSize == LORAWAN_APP_DATA_SIZE) {
                LOG_DEBUG("%-20s: %u.%u", "Timestamp",
                        (uint32_t)(
                                (info->RxBuffer[1] << 24) | (info->RxBuffer[2] << 16)
                                        | (info->RxBuffer[3] << 8) | (info->RxBuffer[4])));
                LOG_DEBUG("TRACE: %-20s: %u", "Latitude", info->RxBuffer[5],
                        (uint32_t)(
                                (info->RxBuffer[6] << 16) | (info->RxBuffer[7] << 8)
                                        | (info->RxBuffer[8])));
                LOG_DEBUG("TRACE: %-20s: %u", "Longitude", info->RxBuffer[9],
                        (uint32_t)(
                                (info->RxBuffer[10] << 16) | (info->RxBuffer[11] << 8)
                                        | (info->RxBuffer[12])));
                LOG_DEBUG_IF(
                        (((info->RxBuffer[0] & SDUHDR_OPTION_LIST_MASK)
                                | SDUHDR_OPTION_LIST_ALT_GPS_MASK) > 0), "TRACE: %-20s: %u",
                        "Altitude (GPS)",
                        (uint16_t)((info->RxBuffer[13] << 8) | info->RxBuffer[14]));
                LOG_DEBUG_IF(
                        (((info->RxBuffer[0] & SDUHDR_OPTION_LIST_MASK)
                                | SDUHDR_OPTION_LIST_ALT_BAR_MASK) > 0), "TRACE: %-20s: %u",
                        "Altitude (bar.)",
                        (uint16_t)((info->RxBuffer[15] << 8) | info->RxBuffer[16]));
                LOG_DEBUG_IF(
                        (((info->RxBuffer[0] & SDUHDR_OPTION_LIST_MASK)
                                | SDUHDR_OPTION_LIST_VEC_TRACK_MASK) > 0), "%-20s: %u",
                        "Ground Speed", (uint16_t)((info->RxBuffer[17] << 8) | info->RxBuffer[18]));
                LOG_DEBUG_IF(
                        (((info->RxBuffer[0] & SDUHDR_OPTION_LIST_MASK)
                                | SDUHDR_OPTION_LIST_VEC_TRACK_MASK) > 0), "%-20s: %u", "Track",
                        (uint16_t)((info->RxBuffer[19] << 8) | info->RxBuffer[20]));
                LOG_DEBUG_IF(
                        (((info->RxBuffer[0] & SDUHDR_OPTION_LIST_MASK)
                                | SDUHDR_OPTION_LIST_WIND_SPEED_MASK) > 0), "%-20s: %u",
                        "Wind Speed", (uint16_t)((info->RxBuffer[21] << 8) | info->RxBuffer[22]));

            }
            break;
        default:
            break;
    }
}

static bool SendFrame(void)
{
    uint8_t sendFrameStatus = 0;

    if (IsTxConfirmed == false) {
        sendFrameStatus = LoRaMacSendFrame(AppPort, AppData, AppDataSize);
    } else {
        sendFrameStatus = LoRaMacSendConfirmedFrame(AppPort, AppData, AppDataSize, 8);
    }

    switch (sendFrameStatus) {
        case 5: // NO_FREE_CHANNEL
            // Try again later
            return true;
        default:
            return false;
    }
}

#if( OVER_THE_AIR_ACTIVATION != 0 )

/*!
 * \brief Function executed on JoinReq Timeout event
 */
static void OnJoinReqTimerEvent( void )
{
    TimerStop( &JoinReqTimer );
    TxNextPacket = true;
}

#endif

/*!
 * \brief Function to be executed on MAC layer event
 */
static void OnMacEvent(LoRaMacEventFlags_t *flags, LoRaMacEventInfo_t *info)
{
    if (flags->Bits.JoinAccept == 1) {
#if( OVER_THE_AIR_ACTIVATION != 0 )
        TimerStop( &JoinReqTimer );
#endif
        IsNetworkJoined = true;
    } else {
        if (flags->Bits.Tx == 1) {
        }

        if (flags->Bits.Rx == 1) {
            if (flags->Bits.RxData == true) {
                ProcessRxFrame(flags, info);
            }
        }
    }
    // Schedule a new transmission
    ScheduleNextTx = true;
}

/*!
 * \brief Function executed on TxNextPacket Timeout event
 */
static void OnTxNextPacketTimerEvent(void)
{
    TimerStop(&TxNextPacketTimer);
    TxNextPacket = true;
}

/**
 * Main application entry point.
 */
int main(void)
{
#if( OVER_THE_AIR_ACTIVATION != 0 )
    uint8_t sendFrameStatus = 0;
#endif
    bool trySendingFrameAgain = false;

    BoardInitMcu();
    LOG_DEBUG("Mcu initialized.");
    BoardInitPeriph();
    LOG_DEBUG("Peripherals initialized.");

    LoRaMacCallbacks.MacEvent = OnMacEvent;
    LoRaMacCallbacks.GetBatteryLevel = BoardGetBatteryLevel;
    LoRaMacInit(&LoRaMacCallbacks);
    LOG_DEBUG("LoRaMesh initialized.");

    IsNetworkJoined = false;

#if( OVER_THE_AIR_ACTIVATION == 0 )
    // NwkAddr
    DevAddr = 0x013D02AB;

    LoRaMacInitNwkIds( LORAWAN_NETWORK_ID, DevAddr, NwkSKey, AppSKey);
    LOG_DEBUG("LoRaMesh network IDs initialized. Network ID: %u, DevAddr: 0x%08x.",
    LORAWAN_NETWORK_ID, DevAddr);
    IsNetworkJoined = true;
#else
    // Initialize LoRaMac device unique ID
    BoardGetUniqueId( DevEui );

    // Sends a JoinReq Command every OVER_THE_AIR_ACTIVATION_DUTYCYCLE
    // seconds until the network is joined
    TimerInit( &JoinReqTimer, OnJoinReqTimerEvent );
    TimerSetValue( &JoinReqTimer, OVER_THE_AIR_ACTIVATION_DUTYCYCLE );
#endif

    TxNextPacket = true;
    TimerInit(&TxNextPacketTimer, OnTxNextPacketTimerEvent);

    LoRaMacSetAdrOn( LORAWAN_ADR_ON);
    LoRaMacTestSetDutyCycleOn( LORAWAN_DUTYCYCLE_ON);
    LoRaMacSetPublicNetwork( LORAWAN_PUBLIC_NETWORK);
//    LoRaMacSetDeviceClass (CLASS_C);

    LOG_DEBUG("Starting LoRa Mesh application...");

    while (1) {
        while (IsNetworkJoined == false) {
#if( OVER_THE_AIR_ACTIVATION != 0 )
            if( TxNextPacket == true )
            {
                TxNextPacket = false;

                sendFrameStatus = LoRaMacJoinReq( DevEui, AppEui, AppKey );
                switch( sendFrameStatus )
                {
                    case 1: // BUSY
                    break;
                    case 0:// OK
                    case 2:// NO_NETWORK_JOINED
                    case 3:// LENGTH_PORT_ERROR
                    case 4:// MAC_CMD_ERROR
                    case 6:// DEVICE_OFF
                    default:
                    // Relaunch timer for next trial
                    TimerStart( &JoinReqTimer );
                    break;
                }
            }
            TimerLowPowerHandler( );
#endif
        }

        if (ScheduleNextTx) {
            ScheduleNextTx = false;

            // Schedule next packet transmission
            TxDutyCycleTime = APP_TX_DUTYCYCLE + randr(-APP_TX_DUTYCYCLE_RND, APP_TX_DUTYCYCLE_RND);
            TimerSetValue(&TxNextPacketTimer, TxDutyCycleTime);
            TimerStart(&TxNextPacketTimer);
        }

        if (trySendingFrameAgain == true) {
            LOG_TRACE("Re-sending frame...");
            trySendingFrameAgain = SendFrame();
            LOG_TRACE_IF(trySendingFrameAgain, "No free channel. Try again later.");
        }

        if (TxNextPacket) {
            LOG_TRACE("Trying to send frame...");
            TxNextPacket = false;

            PrepareTxFrame(AppPort);

            trySendingFrameAgain = SendFrame();

            LOG_TRACE_IF(trySendingFrameAgain, "No free channel. Try again later.");
        }

        TimerLowPowerHandler();
    }
}
