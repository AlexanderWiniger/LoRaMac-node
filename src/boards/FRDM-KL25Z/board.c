/**
 * \file board.c
 * \author Alexander Winiger (alexander.winiger@hslu.ch)
 * \date 15.09.2015
 * \brief Target board general functions implementation
 *
 */

#include "board.h"
#include "fsl_clock_manager.h"
#include "fsl_smc_hal.h"
#include "fsl_debug_console.h"

/*!
 * LED GPIO pins objects
 */
Gpio_t Led1;
Gpio_t Led2;
Gpio_t Led3;

/*!
 * IRQ GPIO pins objects
 */
Gpio_t Irq1Mma8451;
Gpio_t Irq2Mma8451;

/*!
 * MCU objects
 */
Adc_t Adc;
I2c_t I2c;
Uart_t Uart1;
#if defined( USE_USB_CDC )
Uart_t UartUsb;
#endif

/*!
 * Default clock configurations
 */
/* Configuration for enter VLPR mode. Core clock = 4MHz. */
const clock_manager_user_config_t g_defaultClockConfigVlpr =
{
    .mcgConfig =
    {
        .mcg_mode = kMcgModeBLPI,   // Work in BLPI mode.
        .irclkEnable = true,// MCGIRCLK enable.
        .irclkEnableInStop = false,// MCGIRCLK disable in STOP mode.
        .ircs = kMcgIrcFast,// Select IRC4M.
        .fcrdiv = 0U,// FCRDIV is 0.

        .frdiv = 0U,
        .drs = kMcgDcoRangeSelLow,// Low frequency range
        .dmx32 = kMcgDmx32Default,// DCO has a default range of 25%

        .pll0EnableInFllMode = false,// PLL0 disable
        .pll0EnableInStop = false,// PLL0 disalbe in STOP mode
        .prdiv0 = 0U,
        .vdiv0 = 0U,
    },
    .simConfig =
    {
        .pllFllSel = kClockPllFllSelFll, // PLLFLLSEL select FLL.
        .er32kSrc = kClockEr32kSrcLpo,// ERCLK32K selection, use LPO.
        .outdiv1 = 0U,
        .outdiv4 = 4U,
    },
    .oscerConfig =
    {
        .enable = true,  // OSCERCLK enable.
        .enableInStop = false,// OSCERCLK disable in STOP mode.
    }
};

/* Configuration for enter RUN mode. Core clock = 48MHz. */
const clock_manager_user_config_t g_defaultClockConfigRun =
{
    .mcgConfig =
    {
        .mcg_mode = kMcgModePEE,   // Work in PEE mode.
        .irclkEnable = true,// MCGIRCLK enable.
        .irclkEnableInStop = false,// MCGIRCLK disable in STOP mode.
        .ircs = kMcgIrcSlow,// Select IRC32k.
        .fcrdiv = 0U,// FCRDIV is 0.

        .frdiv = 3U,
        .drs = kMcgDcoRangeSelLow,// Low frequency range
        .dmx32 = kMcgDmx32Default,// DCO has a default range of 25%

        .pll0EnableInFllMode = false,// PLL0 disable
        .pll0EnableInStop = false,// PLL0 disalbe in STOP mode
        .prdiv0 = 0x1U,
        .vdiv0 = 0x0U,
    },
    .simConfig =
    {
        .pllFllSel = kClockPllFllSelPll,    // PLLFLLSEL select PLL.
        .er32kSrc = kClockEr32kSrcLpo,// ERCLK32K selection, use LPO.
        .outdiv1 = 1U,
        .outdiv4 = 3U,
    },
    .oscerConfig =
    {
        .enable = true,  // OSCERCLK enable.
        .enableInStop = false,// OSCERCLK disable in STOP mode.
    }
};

/*!
 * Initializes the unused GPIO to a known status
 */
static void BoardUnusedIoInit(void);

/*!
 * Flag to indicate if the MCU is Initialized
 */
static bool McuInitialized = false;

void BoardInitPeriph(void)
{
    /* Init the GPIO extender pins */
    GpioInit(&Irq1Mma8451, IRQ_1_MMA8451, PIN_INPUT, PIN_PUSH_PULL, PIN_NO_PULL, 1);
    GpioInit(&Irq2Mma8451, IRQ_2_MMA8451, PIN_INPUT, PIN_PUSH_PULL, PIN_NO_PULL, 1);
    GpioInit(&Led1, LED_1, PIN_OUTPUT, PIN_PUSH_PULL, PIN_NO_PULL, 0);
    GpioInit(&Led2, LED_2, PIN_OUTPUT, PIN_PUSH_PULL, PIN_NO_PULL, 0);
    GpioInit(&Led3, LED_3, PIN_OUTPUT, PIN_PUSH_PULL, PIN_NO_PULL, 0);

    // Switch LED 1, 2, 3 OFF
    GpioWrite(&Led1, 1);
    GpioWrite(&Led2, 1);
    GpioWrite(&Led3, 1);
}

void BoardInitMcu(void)
{
    Gpio_t ioPin;

    if (McuInitialized == false) {
        /* Set allowed power mode, allow all. */
        SMC_HAL_SetProtection(SMC, kAllowPowerModeAll);

        /* Setup board clock source. */
        // Setup OSC0 if used.
        // Configure OSC0 pin mux.
        GpioInit(&ioPin, OSC_EXTAL0, PIN_ANALOGIC, PIN_PUSH_PULL, PIN_NO_PULL, 1);
        GpioInit(&ioPin, OSC_XTAL0, PIN_ANALOGIC, PIN_PUSH_PULL, PIN_NO_PULL, 1);

        /* Function to initialize OSC0 base on board configuration. */
        osc_user_config_t
        osc0Config =
        {
            .freq = OSC0_XTAL_FREQ,
            .hgo = MCG_HGO0,
            .range = MCG_RANGE0,
            .erefs = MCG_EREFS0,
            .enableCapacitor2p = OSC0_SC2P_ENABLE_CONFIG,
            .enableCapacitor4p = OSC0_SC4P_ENABLE_CONFIG,
            .enableCapacitor8p = OSC0_SC8P_ENABLE_CONFIG,
            .enableCapacitor16p = OSC0_SC16P_ENABLE_CONFIG,
        };

        CLOCK_SYS_OscInit(0U, &osc0Config);

        // Setup RTC external clock if used.
#if RTC_XTAL_FREQ
        // If RTC_CLKIN is connected, need to set pin mux. Another way for
        // RTC clock is set RTC_OSC_ENABLE_CONFIG to use OSC0, please check
        // reference manual for datails.
        PORT_HAL_SetMuxMode(RTC_CLKIN_PORT, RTC_CLKIN_PIN, RTC_CLKIN_PINMUX);
#endif
#if ((OSC0_XTAL_FREQ != 32768U) && (RTC_OSC_ENABLE_CONFIG))
#error Set RTC_OSC_ENABLE_CONFIG will override OSC0 configuration and OSC0 must be 32k.
#endif
        rtc_osc_user_config_t
        rtcOscConfig =
        {
            .freq = RTC_XTAL_FREQ,
            .enableCapacitor2p = RTC_SC2P_ENABLE_CONFIG,
            .enableCapacitor4p = RTC_SC4P_ENABLE_CONFIG,
            .enableCapacitor8p = RTC_SC8P_ENABLE_CONFIG,
            .enableCapacitor16p = RTC_SC16P_ENABLE_CONFIG,
            .enableOsc = RTC_OSC_ENABLE_CONFIG,
            .enableClockOutput = RTC_CLK_OUTPUT_ENABLE_CONFIG,
        };

        CLOCK_SYS_RtcOscInit(0U, &rtcOscConfig);

        /* Set system clock configuration. */
#if (CLOCK_INIT_CONFIG == CLOCK_VLPR)
        CLOCK_SYS_SetConfiguration (&g_defaultClockConfigVlpr);
#else
        CLOCK_SYS_SetConfiguration(&g_defaultClockConfigRun);
#endif

        TimerSetLowPowerEnable(false);

        BoardUnusedIoInit();

        if (TimerGetLowPowerEnable() == true) {
            RtcInit();
        } else {
            TimerHwInit();
        }
        McuInitialized = true;
    }
}

void BoardDeInitMcu(void)
{
    Gpio_t ioPin;

    I2cDeInit(&I2c);
    SpiDeInit(&SX1276.Spi);
    SX1276IoDeInit();

    GpioInit(&ioPin, OSC_EXTAL0, PIN_ANALOGIC, PIN_PUSH_PULL, PIN_NO_PULL, 1);
    GpioInit(&ioPin, OSC_XTAL0, PIN_ANALOGIC, PIN_PUSH_PULL, PIN_NO_PULL, 1);

    McuInitialized = false;
}

void BoardGetUniqueId(uint8_t *id)
{

}

static void BoardUnusedIoInit(void)
{

}
