/*
 * clidemo.c
 *
 *  Created on: May 20, 2017
 *      Author: julian
 */

#include "libopencm3/stm32/rcc.h"
#include "libopencm3/cm3/systick.h"

#include "bsp/bsp.h"
#include "bsp/bsp_gpio.h"
#include "bsp/bsp_usart.h"

#include "mybuildroot/common.h"
#include "cli.h"

#include <stdio.h>
#include <stdint.h>

#define VERSIONSTRING      "rel_1_0_0"

#define LED_PORT            GPIOA
#define LED_PIN             GPIO5

typedef enum
{
	LED_STATIC = 0,
	LED_BLINK

}led_mode_t;

/**
 * We need a global command line interface instance.
 */
Cli cli;

/**
 * The mode of the led
 */
led_mode_t ledMode = LED_BLINK;

/**
 * Systick counter, one tick = 1ms
 */
volatile uint32_t sysTick = 0;

static void systick_setup(void)
{
    /* 72MHz / 8 => 9000000 counts per second */
    systick_set_clocksource(STK_CSR_CLKSOURCE_AHB_DIV8);

    /*
     * 9000000/9000 = 1000 overflows per second - every 1ms one interrupt
     * SysTick interrupt every N clock pulses: set reload to N-1
     * */
    systick_set_reload(8999);

    systick_interrupt_enable();
    systick_counter_enable();
}

/**
 * As this is a C++ project we have to prevent the compiler from mangling the
 * function name with arguments to support function overloading. Doing so
 * allows to link with external C code.
 */
extern "C"
void sys_tick_handler(void)
{
    sysTick++;
}

/**
 * to print the version.
 */
int8_t cmd_ver(void* args)
{
	unused(args);

	printf(VERSIONSTRING "\n");
    printf("build: %s, %s\n", __DATE__, __TIME__);
    return 0;
}

/**
 * To control the led
 */
int8_t cmd_led(void* args)
{
	int8_t ret = 0;
	char *c = cli.get_parg();

	unused(args);

	if (c == 0)
	{
		ret = -1;
	    goto out;
	}

	if (*c == '0')
	{
		ledMode = LED_STATIC;
		bspLedOff();
	}
	else if (*c == '1')
	{
		ledMode = LED_STATIC;
		bspLedOn();
	}
	else if (*c == 'b')
	{
		ledMode = LED_BLINK;
	}

	out:
	return ret;
}

/**
 * The table of supported commands.
 */
cli_cmd_t cmd_table[] =
{
   {"ver", cmd_ver}, /* print the version */
   {"led", cmd_led}, /* to control the led */
   {0,      0}
};

int main(void)
{
    uint32_t lastTick = 0;

    bspClockSetup(BSP_HSE_BYPASS_ON, BSP_CPU_SPEED_72MHZ);
    bspLedInit();
    bspTTYInit(115200);
    systick_setup();

    printf("Command line interface demo.\n");
    printf(VERSIONSTRING"\n");
    printf("build: %s, %s\n", __DATE__, __TIME__);
    printf("Buffersize: %d\n", BUFSIZ);

    /* Start the commandline interface */
    cli.init(cmd_table, arraysize(cmd_table));

    while (1)
    {
        if (sysTick - lastTick > 250)
        {
            lastTick = sysTick;

            if (ledMode == LED_BLINK)
            {
                bspLedToggle();
            }
        }

        if (bspTTYDataAvailable())
        {
        	/* pass rx data to the cli  */
        	cli.proc_byte((uint8_t) bspTTYGetChar());
        }
    }

    return 0;
}
