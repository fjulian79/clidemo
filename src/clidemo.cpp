/*
 * clidemo, a example and test bench for my command line library libcli.
 *
 * Copyright (C) 2020 Julian Friedrich
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>. 
 *
 * You can file issues at https://github.com/fjulian79/clidemo/issues
 */

#include "bsp/bsp.h"
#include "bsp/bsp_gpio.h"
#include "bsp/bsp_tty.h"
#include "cli/cli.hpp"
#include "generic/generic.hpp"

#include <stdio.h>
#include <stdint.h>

/**
 * @brief The version string of the application.
 * 
 */
#define VERSIONSTRING      "rel_2_0_0"

/**
 * @brief Defines the operational mode of the led.
 */
typedef enum
{
	LED_STATIC = 0,
	LED_BLINK

}led_mode_t;

/**
 * @brief The global command line interface instance.
 */
Cli cli;

/**
 * @brief The operational mode of the led
 */
led_mode_t ledMode = LED_BLINK;

/**
 * @brief Used to print the version informaton.
 * 
 * This function has to match the p_cmd_func definition in libcli, see cliCmd_t.
 * 
 * @param argv      not used.
 * @param argc      not used.
 * @return int8_t   Zero.
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
 * @brief Used to control the on board led
 *
 * Supports one argument in from of a character.
 *      0 ... turns the LED off.
 *      1 ... turns the LED on.
 *      b ... let the LED blink.
 * 
 * This function has to match the p_cmd_func definition in libcli, see cliCmd_t.
 * 
 * @param argv      Array of arguments.
 * @param argc      Number of provided arguments.
 * @return int8_t   Zero.
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
 * @brief This function prints the curent tty configuration.
 * 
 * This function has to match the p_cmd_func definition in libcli, see cliCmd_t.
 * 
 * @param argv      not used.
 * @param argc      not used.
 * @return int8_t   Zero.
 */
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

/**
 * @brief This function return the value passed to it to test the error 
 * detection in libcli.
 * 
 * This function has to match the p_cmd_func definition in libcli, see cliCmd_t.
 *
 * @param argv      Array of arguments.
 * @param argc      Number of arguments.
 * @return int8_t   The provided value.
 */
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

/**
 * @brief The function will print all provided arguments.
 * 
 * This function has to match the p_cmd_func definition in libcli, see cliCmd_t.
 * 
 * @param argv      Array of arguments.
 * @param argc      Number of arguments.
 * @return int8_t   Zero.
 */
int8_t cmd_list(char *argv[], uint8_t argc)
{
    printf("Recognized arguments:\n");
    for(size_t i = 0; i < argc; i++)
    {
        printf("  argv[%d]: \"%s\"\n", i, argv[i]);
    }
    
    return 0;
}

int8_t cmd_reset(char *argv[], uint8_t argc)
{
    unused(argv);
    unused(argc);

    printf("Resetting the CPU ...\n\n");
    bspDelayMs(100);

    bspResetCpu();

    return 0;
}

/**
 * @brief To to print the help text.
 * 
 * This function has to match the p_cmd_func definition in libcli, see cliCmd_t.
 * 
 * @param argv      not used.
 * @param argc      not used.
 * @return int8_t   Zero.
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
    printf("  cfg         Used to print the tty configuration.\n");
    printf("  err ret     Used to test errors in a command.\n");
    printf("                ret   return value of the called function.\n");
    printf("  list [args] Used to test how arguments are parsed.\n");
    printf("                args  a list of arguments.\n");
    printf("  reset       Used to reset the CPU.\n");
    printf("  help        Prints this text.\n");

    return 0;
}

/**
 * @brief The table of supported commands.
 */
cliCmd_t cmdTable[] =
{
   {"ver", cmd_ver},
   {"led", cmd_led},
   {"cfg", cmd_cfg},
   {"err", cmd_err},
   {"list", cmd_list},
   {"reset", cmd_reset},
   {"help", cmd_help},
};

int main(void)
{
    uint32_t lastTick = 0;
    uint32_t tick = 0;

    bspChipInit();

    printf("Command line interface demo.\n");
    cmd_ver(0, 0);

    /* Initialize the command line interface */
    cli.init(cmdTable, arraysize(cmdTable));

    while (1)
    {
        tick = bspGetSysTick();

        if (tick - lastTick >= 250)
        {
            lastTick = tick;
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
