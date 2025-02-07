/*
 * clidemo, a example and test bench for my command line library libcli.
 *
 * Copyright (C) 2025 Julian Friedrich
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
 * This project is hosted on GitHub:
 *   https://github.com/fjulian79/clidemo
 * Please feel free to file issues, open pull requests, or contribute there.
 */

#include <Arduino.h>
#include <cli/cli.hpp>
#include <generic/generic.hpp>
#include <generic/task.hpp>

#include "version/version.h"

#include <stdio.h>
#include <stdint.h>

#ifdef ARDUINO_ARCH_ESP8266 
/**
 * @brief Currently there is no common code to get the rx buffer size of the
 * default serial port, so we need platform dependant code here.
 * 
 */
#define SERIAL_RX_BUFFER_SIZE       Serial.getRxBufferSize()
#endif

/**
 * @brief Needed for ESP32 and RP2040 build tarets.
 */
#ifndef SERIAL_RX_BUFFER_SIZE

/**
 * @brief Currently there is no common code to get the rx buffer size of the
 * default serial port, so we need platform dependant code here.
 * 
 */
#define SERIAL_RX_BUFFER_SIZE       0
#endif

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
 * @brief To blink the led .. wohoo
 */
Task ledTask(250);
Task serialTask(10);

/**
 * @brief Used to print version information.
 */
CLI_COMMAND(ver)
{
    ioStream.printf("\n%s %s, Copyright (C) 2025 Julian Friedrich\n", 
            VERSION_PROJECT, VERSION_GIT_SHORT);
    ioStream.printf("Build:    %s, %s\n", __DATE__, __TIME__);
    ioStream.printf("Git Repo: %s\n", VERSION_GIT_REMOTE_ORIGIN);
    ioStream.printf("Revision: %s\n", VERSION_GIT_LONG);
    ioStream.printf("\n");
    ioStream.printf("This program comes with ABSOLUTELY NO WARRANTY. This is free software, and you\n");
    ioStream.printf("are welcome to redistribute it under certain conditions.\n");
    ioStream.printf("See GPL v3 licence at https://www.gnu.org/licenses/ for details.\n\n");

    return 0;
}

/**
 * @brief Used to control the on board led
 * @arg   mode  0|1|b
 */
CLI_COMMAND(led)
{	
	if (argc != 1)
	{
		return -1;
	}

	if (*argv[0] == '0')
	{
		ledMode = LED_STATIC;
		digitalWrite(LED_BUILTIN, 0);
	}
	else if (*argv[0] == '1')
	{
		ledMode = LED_STATIC;
		digitalWrite(LED_BUILTIN, 1);
	}
	else if (*argv[0] == 'b')
	{
		ledMode = LED_BLINK;
	}

	return 0;
}

/**
 * @brief Prints infos on lib cli
 */
CLI_COMMAND(info)
{
    cliCmd_t *pCmdTab = CliCommand::getTable();
    size_t cmdCnt = CliCommand::getCmdCnt();
    size_t dropCnt = CliCommand::getDropCnt();

    ioStream.printf("  CLI_COMMANDSIZ:        %d\n", CLI_COMMANDSIZ);
    ioStream.printf("  CLI_ARGVSIZ:           %d\n", CLI_ARGVSIZ);
    ioStream.printf("  CLI_PROMPT:            %s\n", CLI_PROMPT);
    ioStream.printf("  CLI_BUFFEREDIO:        %d\n", CLI_BUFFEREDIO);
    ioStream.printf("  SERIAL_RX_BUFFER_SIZE: %d\n", SERIAL_RX_BUFFER_SIZE);
    ioStream.printf("  Supported commands:    %d\n", CLI_COMMANDS_MAX);
    ioStream.printf("  Registered commands:   %d\n", cmdCnt);
    ioStream.printf("  Dropped commands:      %d\n", dropCnt);
    ioStream.printf("  Command name's:\n");

    for(size_t i = 0; i < cmdCnt; i++)
    {
        ioStream.printf("    %s\n", pCmdTab[i].name);
    }  
    ioStream.print("\n");

    return 0;
}

/**
 * @brief Used to test errors in a command.
 * @arg   ret return value of the command
 */
CLI_COMMAND(err)
{
    int16_t val = -1;

    if (argc > 0)
    {
        val = strtol(argv[0],0 ,0);
        ioStream.printf("Got value %d\n", val);
    }

    return (int8_t) val;
}

/**
 * @brief Prints all provided arguments.
 * @arg   [args] Optional list of arguments.
 */
CLI_COMMAND(args)
{
    ioStream.printf("Recognized arguments:\n");
    for(size_t i = 0; i < argc; i++)
    {
        ioStream.printf("  argv[%d]: \"%s\"\n", i, argv[i]);
    }
    
    return 0;
}

/**
 * @brief Used to change the echo mode of libcli
 * @arg   mode on|off
 */
CLI_COMMAND(echo)
{
    bool state = true;

    if(argc != 1)
    {
        return -1;
    }

    if(strcmp(argv[0],"on") == 0)
    {
        state = true;
    }
    else if(strcmp(argv[0],"off") == 0)
    {
        state = false;
    }
    else
    {
        return -2;
    }

    cli.setEcho(state);
    ioStream.printf("Echo is now %s\n", argv[0]);

    return 0; 
}

/**
 * @brief Rings the bell in the host terminal.
 */
CLI_COMMAND(bell)
{
    cli.sendBell();
    ioStream.printf("Sent a bell cmd\n");

    return 0; 
}

/**
 * @brief Trigger a CPU reset.
 */
CLI_COMMAND(reset)
{
    ioStream.printf("Resetting the CPU ...\n");
    delay(100);

    #ifdef ARDUINO_ARCH_STM32
    
    NVIC_SystemReset();
    
    #elif ARDUINO_ARCH_ESP32 || ARDUINO_ARCH_ESP8266
    
    ESP.restart();
    
    #elif ARDUINO_ARCH_RP2040

    rp2040.reboot();

    #else

    ioStream.printf("ERROR: reset is not implemented for this platform.\n");

    #endif
    
    return 0;
}

/**
 * @brief Print's the help text.
 */
CLI_COMMAND(help)
{
    ioStream.printf("Supported commands:\n");
    ioStream.printf("  ver            Used to print version infos.\n");
    ioStream.printf("  led mode       Used to control the led. \n");
    ioStream.printf("                   Supported modes:\n");
    ioStream.printf("                   0 ... turns the led off.\n");
    ioStream.printf("                   1 ... turns the led on.\n");
    ioStream.printf("                   b ... let it blink.\n");
    ioStream.printf("  info           Used to print lib cli infos.\n");
    ioStream.printf("  err ret        Used to test errors in a command.\n");
    ioStream.printf("                   ret   return value of the called function.\n");
    ioStream.printf("  list [args]    Used to test how arguments are parsed.\n");
    ioStream.printf("                   args  a list of arguments.\n");
    ioStream.printf("  bell           Used to ring the bell of the host terminal.\n");
    ioStream.printf("  echo on|off    Used to turn echo on or off.\n");
    ioStream.printf("  reset          Used to reset the CPU.\n");
    ioStream.printf("  help           Prints this text.\n");

    return 0;
}

/**
 * @brief Dummy command to check the dropCnt.
 * 
 * The number of supporded commands is set to 9 in platformio.ini. So this one 
 * here shold be dropped. Can be checked with the info command.
 */
CLI_COMMAND(dummy)  
{
    ioStream.printf("I'm used do check the dropCnt.\n");
    return 0;
}

/**
 * This task makes only sense on a MCU's with build in USB CDC Serial which
 * is aware of active connections. On all other boards it will work as well,
 * but transition to state 2 and stay there for ever.
 */
void serialTaskFunction(uint32_t now)
{
    static uint8_t state = 0;

    if (Serial && state == 0)
    {
        state = 1;
        return;
    }

    if (state == 1)
    {
        state = 2;
        Serial.println();
        cmd_ver(Serial, 0, 0);
        cli.begin();
    }

    if (!Serial && state != 0)
    {
        state = 0;
    }

    if(state == 2)
    {
        cli.loop();
    }
}

void setup()
{
    pinMode(LED_BUILTIN, OUTPUT);
    
    Serial.begin(115200);  
    serialTask.setTaskFunction(serialTaskFunction);
}

void loop()
{
    uint32_t now = millis();

    if(ledTask.isScheduled(now))
    {
        if (ledMode == LED_BLINK)
        {
            digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
        }
    }

    serialTask.loop(now);
}