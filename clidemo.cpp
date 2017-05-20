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

/**
 * Nucleo-F103RB led definitions
 */
#define LED_PORT            GPIOA
#define LED_PIN             GPIO5

/**
 * Our systick counter, one tick = 1ms
 */
volatile uint32_t sysTick = 0;

static void clock_setup(void)
{
    /*
     * We want to use full speed .. check that the clock from the st-link
     * is provided to the mcu.
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
    usart_set_mode(USART2, USART_MODE_TX);
    usart_set_parity(USART2, USART_PARITY_NONE);
    usart_set_flow_control(USART2, USART_FLOWCONTROL_NONE);

    usart_enable(USART2);
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

extern "C"
{

void sys_tick_handler(void)
{
    sysTick++;
}

int _write(int file, char *ptr, int len)
{
	int i;

	if (file == 1) {
		for (i = 0; i < len; i++)
			usart_send_blocking(USART2, ptr[i]);
		return i;
	}

	errno = EIO;
	return -1;
}

} /* extern "C"  */

#define THRUSTMETER_VERSIONSTRING      "clidemo rel_1_0_0\n"

/**
 * Just say who we are
 */
int8_t cmd_ver(void* args)
{
	unused(args);

	printf(THRUSTMETER_VERSIONSTRING);
    printf("build: %s, %s\n", __DATE__, __TIME__);
    return 0;
}

/**
 * The table of supported commands.
 */
cli_cmd_t cmd_table[] =
{
   {"ver", 	cmd_ver}, /* print the version */
   {0,      0}
};

int main(void)
{
    Cli cli;
    uint32_t lastTick = 0;
    uint16_t c = 0;

    clock_setup();
    gpio_setup();
    usart_setup();
    systick_setup();

    cli.init(cmd_table, arraysize(cmd_table));

    printf("Hello World! %d\r\n", c);

    while (1)
    {
        if (sysTick - lastTick > 250)
        {
            gpio_toggle(LED_PORT, LED_PIN);
            lastTick = sysTick;

            printf("Hello World! %d\r\n", c);
            c = c < 9 ? c+1 : 0;
        }

        if (usart_get_flag(USART2,USART_SR_RXNE))
        {
        	cli.proc_byte((uint8_t)usart_recv(USART2));
        }
    }

    return 0;
}
