/**
 * \file main.c
 * \author Alexander Winiger (alexander.winiger@hslu.ch)
 * \date 09.11.2015
 * \brief FreeRTOS test implementation
 *
 */

#include "board.h"
#include "fsl_os_abstraction.h"

// task prio
#define TASK_LPM_PRIO                6U
#define TASK_LED_RTOS_PRIO           7U
#define TASK_LED_CLOCK_PRIO          8U

#define TASK_LPM_STACK_SIZE          0x500U
#define TASK_LED_RTOS_STACK_SIZE     0x200U
#define TASK_LED_CLOCK_STACK_SIZE    0x200U

extern void * g_uartStatePtr[UART_INSTANCE_COUNT];
extern void UART_DRV_IRQHandler(uint32_t instance);

void PM_DBG_UART_IRQ_HANDLER(void)
{
    UART_DRV_IRQHandler(BOARD_DEBUG_UART_INSTANCE);
}

extern void PM_DBG_UART_IRQ_HANDLER(void);

// task declare
extern void task_lpm(task_param_t param);
extern void task_led_rtos(task_param_t param);

// task define
OSA_TASK_DEFINE(task_lpm, TASK_LPM_STACK_SIZE);
OSA_TASK_DEFINE(task_led_rtos, TASK_LED_RTOS_STACK_SIZE);

static uart_state_t s_dbgState;
static osa_status_t s_result = kStatus_OSA_Error;

int main(void)
{
    memset(&s_dbgState, 0, sizeof(s_dbgState));

    hardware_init();
    OSA_Init();

    /* Init the interrupt sync object. */
    OSA_SemaCreate(&s_dbgState.txIrqSync, 0);
    OSA_SemaCreate(&s_dbgState.rxIrqSync, 0);
    //init the uart module with base address and config structure
    g_uartStatePtr[BOARD_DEBUG_UART_INSTANCE] = &s_dbgState;
    NVIC_EnableIRQ(g_uartRxTxIrqId[BOARD_DEBUG_UART_INSTANCE]);

    // Initializes GPIO driver for LEDs and buttons
    GPIO_DRV_Init(switchPins, ledPins);

    NVIC_SetPriority(PM_DBG_UART_IRQn, 6U);

    NVIC_SetPriority(RTC_IRQn, 6U);
    NVIC_SetPriority(LPTMR0_IRQn, 6U);
    NVIC_SetPriority(LLWU_IRQn, 6U);

    // Low power manager task.
    s_result = OSA_TaskCreate(task_lpm,
                (uint8_t *)"lpm",
                TASK_LPM_STACK_SIZE,
                task_lpm_stack,
                TASK_LPM_PRIO,
                (task_param_t)0,
                false,
                &task_lpm_task_handler);
    if (s_result != kStatus_OSA_Success)
    {
         PRINTF("Failed to create lpm task\r\n");
    }

    // These tasks will not start in BM.
    s_result = OSA_TaskCreate(task_led_rtos,
                (uint8_t *)"led_rtos",
                TASK_LED_RTOS_STACK_SIZE,
                task_led_rtos_stack,
                TASK_LED_RTOS_PRIO,
                (task_param_t)0,
                false,
                &task_led_rtos_task_handler);
    if (s_result != kStatus_OSA_Success)
    {
        PRINTF("Failed to create led_rtos task\r\n");
    }

    OSA_Start();

    for(;;) {}                    // Should not achieve here
}
