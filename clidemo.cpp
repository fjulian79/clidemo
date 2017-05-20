/*
 * clidemo.c
 *
 *  Created on: May 20, 2017
 *      Author: julian
 */

#include "libopencm3/stm32/rcc.h"
#include "libopencm3/stm32/gpio.h"
#include "libopencm3/stm32/usart.h"
#include "libopencm3/cm3/systick.h"

#include "mybuildroot/common.h"
#include "cli.h"

#include <stdio.h>
#include <stdint.h>
#include <errno.h>

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

static void clock_setup(void)
{
    /*
     * We want to use full speed. Check that the clock from the st-link is
     * provided to the mcu.
     * */
    rcc_osc_off(RCC_HSE);
    rcc_osc_bypass_enable(RCC_HSE);
    rcc_clock_setup_in_hse_8mhz_out_72mhz();
}

static void gpio_setup(void)
{
    rcc_periph_clock_enable(RCC_GPIOA);

    gpio_set_mode(LED_PORT, GPIO_MODE_OUTPUT_2_MHZ,
              GPIO_CNF_OUTPUT_PUSHPULL, LED_PIN);

    gpio_set_mode(GPIOA, GPIO_MODE_OUTPUT_50_MHZ,
              GPIO_CNF_OUTPUT_ALTFN_PUSHPULL, GPIO_USART2_TX);
    gpio_set_mode(GPIOA, GPIO_MODE_INPUT,
              GPIO_CNF_INPUT_FLOAT, GPIO_USART2_RX);
}

static void usart_setup(void)
{
    rcc_periph_clock_enable(RCC_USART2);

    usart_set_baudrate(USART2, 115200);
    usart_set_databits(USART2, 8);
    usart_set_stopbits(USART2, USART_STOPBITS_1);
    usart_set_mode(USART2, USART_MODE_TX_RX);
    usart_set_parity(USART2, USART_PARITY_NONE);
    usart_set_flow_control(USART2, USART_FLOWCONTROL_NONE);

    usart_enable(USART2);

    /**
     * stdout is line buffered by default. This causes the cli prompt not to
     * be printed when calling printf without line termination.
     */
    setbuf(stdout, NULL);
}

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

extern "C"
int _write(int file, char *ptr, int len)
{
	int i;

	if (file == 1)
	{
		for (i = 0; i < len; i++)
			usart_send_blocking(USART2, ptr[i]);
		return i;
	}

	errno = EIO;
	return -1;
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

	if (c == 0)
	{
		ret = -1;
	    goto out;
	}

	if (*c == '0')
	{
		ledMode = LED_STATIC;
		gpio_clear(LED_PORT, LED_PIN);
	}
	else if (*c == '1')
	{
		ledMode = LED_STATIC;
		gpio_set(LED_PORT, LED_PIN);
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

    clock_setup();
    gpio_setup();
    usart_setup();
    systick_setup();

    printf("Command line interface demo.\n");
    printf(VERSIONSTRING"\n");

    /* Start the commandline interface */
    cli.init(cmd_table, arraysize(cmd_table));

    while (1)
    {
        if (sysTick - lastTick > 250)
        {
            lastTick = sysTick;

            if (ledMode == LED_BLINK)
            {
            	gpio_toggle(LED_PORT, LED_PIN);
            }
        }

        if (usart_get_flag(USART2,USART_SR_RXNE) == true)
        {
        	/* pass rx data to the cli  */
        	cli.proc_byte((uint8_t) usart_recv(USART2));
        }
    }

    return 0;
}
