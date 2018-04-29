/*
 * clidemo.c
 *
 *  Created on: May 20, 2017
 *      Author: julian
 */

#include "bsp/bsp.h"
#include "bsp/bsp_gpio.h"
#include "bsp/bsp_tty.h"
#include "cli/cli.h"
#include "generic/generic.h"

#include <stdio.h>
#include <stdint.h>

#define VERSIONSTRING      "rel_2_0_0"

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
 * to print the version.
 */
int8_t cmd_ver(void* args)
{
	unused(args);

	printf("version: %s\n", VERSIONSTRING);
    printf("build:   %s, %s\n", __DATE__, __TIME__);
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
		bspGpioClear(BSP_GPIO_LED);
	}
	else if (*c == '1')
	{
		ledMode = LED_STATIC;
		bspGpioSet(BSP_GPIO_LED);
	}
	else if (*c == 'b')
	{
		ledMode = LED_BLINK;
	}

	out:
	return ret;
}

/**
 * To to print the help text
 */
int8_t cmd_help(void* args)
{
    unused(args);

    printf("Supported commands:\n");
    printf("  ver       Used to print version infos.\n");
    printf("  led mode  Used to control the led. \n");
    printf("              Supported modes:\n");
    printf("              0 ... turns the led off.\n");
    printf("              1 ... turns the led on.\n");
    printf("              b ... let it blink.\n");
    printf("  err ret   Used to test arros in a command.\n");
    printf("              ret   return value.\n");
    printf("  help      Prints this text.\n");

    return 0;
}

int8_t cmd_cfg(void* args)
{
    unused(args);

    printf("BSP_TTY_BLOCKING:   %d\n", BSP_TTY_BLOCKING);
    printf("BSP_TTY_TX_BUFSIZ:  %d\n", BSP_TTY_TX_BUFSIZ);
    printf("BSP_TTY_TX_DMA:     %d\n", BSP_TTY_TX_DMA);
    printf("BSP_TTY_RX_BUFSIZ:  %d\n", BSP_TTY_RX_BUFSIZ);
    printf("BSP_TTY_RX_IRQ:     %d\n", BSP_TTY_RX_IRQ);

    return 0;
}

int8_t cmd_err(void* args)
{
    int16_t val = -1;

    if (cli.get_parg())
    {
        cli.arg2int16(&val);
        printf("Got value %d\n", val);
    }

    return (int8_t) val;
}

/**
 * The table of supported commands.
 */
cli_cmd_t cmd_table[] =
{
   {"ver", cmd_ver},    /* Print the version */
   {"led", cmd_led},    /* To control the led */
   {"help", cmd_help},  /* To control the led */
   {"cfg", cmd_cfg},    /* To print the configuration */
   {"err", cmd_err},    /* To print the configuration */
   {0,      0}
};

int main(void)
{
    uint32_t lastTick = 0;

    bspChipInit();

    printf("Command line interface demo.\n");
    cmd_ver(0);

    /* Initialize the command line interface */
    cli.init(cmd_table, arraysize(cmd_table));

    while (1)
    {
        if (sysTick - lastTick >= 250)
        {
            lastTick = sysTick;
            if (ledMode == LED_BLINK)
            {
                bspGpioToggle(BSP_GPIO_LED);
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
