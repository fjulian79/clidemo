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
int8_t cmd_ver(char *argv[], uint8_t argc)
{
    unused(argv);
    unused(argc);

	printf("version: %s\n", VERSIONSTRING);
    printf("build:   %s, %s\n", __DATE__, __TIME__);
    return 0;
}

/**
 * To control the led
 */
int8_t cmd_led(char *argv[], uint8_t argc)
{
	int8_t ret = 0;
	
	if (argc != 1)
	{
		ret = -1;
	    goto out;
	}

	if (*argv[0] == '0')
	{
		ledMode = LED_STATIC;
		bspGpioClear(BSP_GPIO_LED);
	}
	else if (*argv[0] == '1')
	{
		ledMode = LED_STATIC;
		bspGpioSet(BSP_GPIO_LED);
	}
	else if (*argv[0] == 'b')
	{
		ledMode = LED_BLINK;
	}

	out:
	return ret;
}

/**
 * To to print the help text
 */
int8_t cmd_help(char *argv[], uint8_t argc)
{
    unused(argv);
    unused(argc);

    printf("Supported commands:\n");
    printf("  ver         Used to print version infos.\n");
    printf("  led mode    Used to control the led. \n");
    printf("                Supported modes:\n");
    printf("                0 ... turns the led off.\n");
    printf("                1 ... turns the led on.\n");
    printf("                b ... let it blink.\n");
    printf("  err ret     Used to test arros in a command.\n");
    printf("                ret   return value.\n");
    printf("  list [args] Used to test how arguments are parsed.\n");
    printf("                args  a list of arguments.\n");
    printf("  help        Prints this text.\n");

    return 0;
}

int8_t cmd_cfg(char *argv[], uint8_t argc)
{
    unused(argv);
    unused(argc);

    printf("BSP_TTY_BLOCKING:   %d\n", BSP_TTY_BLOCKING);
    printf("BSP_TTY_TX_BUFSIZ:  %d\n", BSP_TTY_TX_BUFSIZ);
    printf("BSP_TTY_TX_DMA:     %d\n", BSP_TTY_TX_DMA);
    printf("BSP_TTY_RX_BUFSIZ:  %d\n", BSP_TTY_RX_BUFSIZ);
    printf("BSP_TTY_RX_IRQ:     %d\n", BSP_TTY_RX_IRQ);

    return 0;
}

int8_t cmd_err(char *argv[], uint8_t argc)
{
    int16_t val = -1;

    if (argc > 0)
    {
        cli.toSigned(argv[0], &val, sizeof(val));
        printf("Got value %d\n", val);
    }

    return (int8_t) val;
}

int8_t cmd_list(char *argv[], uint8_t argc)
{
    printf("Recognized arguments:");
    for(size_t i = 0; i < argc; i++)
    {
        printf("  argv[%d]: %s\n", i, argv[i]);
    }
    
    return 0;
}

/**
 * The table of supported commands.
 */
cliCmd_t cmdTable[] =
{
   {"ver", cmd_ver},
   {"led", cmd_led},
   {"help", cmd_help},
   {"cfg", cmd_cfg},
   {"err", cmd_err},
   {"list", cmd_list},
   {0,      0}
};

int main(void)
{
    uint32_t lastTick = 0;

    bspChipInit();

    printf("Command line interface demo.\n");
    cmd_ver(0, 0);

    /* Initialize the command line interface */
    cli.init(cmdTable, arraysize(cmdTable));

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
        	cli.procByte((uint8_t) bspTTYGetChar());
        }
    }

    return 0;
}
