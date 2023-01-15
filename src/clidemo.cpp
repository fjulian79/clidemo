/*
 * clidemo, a example and test bench for my command line library libcli.
 *
 * Copyright (C) 2023 Julian Friedrich
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

#ifdef ARDUINO_ARCH_ESP32
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

/**
 * @brief Used to print the version informaton.
 */
CLI_COMMAND(ver)
{
    Serial.printf("\n%s %s, Copyright (C) 2021 Julian Friedrich\n", 
            VERSION_PROJECT, VERSION_GIT_SHORT);
    Serial.printf("Build:    %s, %s\n", __DATE__, __TIME__);
    Serial.printf("Git Repo: %s\n", VERSION_GIT_REMOTE_ORIGIN);
    Serial.printf("Revision: %s\n", VERSION_GIT_LONG);
    Serial.printf("\n");
    Serial.printf("This program comes with ABSOLUTELY NO WARRANTY. This is free software, and you\n");
    Serial.printf("are welcome to redistribute it under certain conditions.\n");
    Serial.printf("See GPL v3 licence at https://www.gnu.org/licenses/ for details.\n\n");

    return 0;
}

/**
 * @brief Used to control the on board led
 *
 * Supports one argument in from of a character.
 *      0 ... turns the LED off.
 *      1 ... turns the LED on.
 *      b ... let the LED blink.
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
 * @brief This function prints the curent tty configuration.
 */
CLI_COMMAND(cfg)
{
    Serial.printf(" CLI_COMMANDSIZ:        %d\n", CLI_COMMANDSIZ);
    Serial.printf(" CLI_ARGVSIZ:           %d\n", CLI_ARGVSIZ);
    Serial.printf(" CLI_PROMPT:            %s\n", CLI_PROMPT);
    Serial.printf(" CLI_BUFFEREDIO:        %d\n", CLI_BUFFEREDIO);
    Serial.printf(" SERIAL_RX_BUFFER_SIZE: %d\n\n", SERIAL_RX_BUFFER_SIZE);

    return 0;
}

/**
 * @brief This function return the value passed to it to test the error 
 * detection in libcli.
 */
CLI_COMMAND(err)
{
    int16_t val = -1;

    if (argc > 0)
    {
        val = strtol(argv[0],0 ,0);
        Serial.printf("Got value %d\n", val);
    }

    return (int8_t) val;
}

/**
 * @brief The function will print all provided arguments.
 */
CLI_COMMAND(list)
{
    Serial.printf("Recognized arguments:\n");
    for(size_t i = 0; i < argc; i++)
    {
        Serial.printf("  argv[%d]: \"%s\"\n", i, argv[i]);
    }
    
    return 0;
}

/**
 * @brief Used to change the echo mode of libcli
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
    Serial.printf("Echo is now %s\n", argv[0]);

    return 0; 
}

/**
 * @brief Rings the bell in the host terminal application.
 */
CLI_COMMAND(bell)
{
    cli.sendBell();
    Serial.printf("Sent a bell cmd\n");

    return 0; 
}

/**
 * @brief Trigger a CPU reset on a STM32.
 */
CLI_COMMAND(reset)
{
    Serial.printf("Resetting the CPU ...\n");
    delay(100);

    #ifdef ARDUINO_ARCH_STM32
    
    NVIC_SystemReset();
    
    #elif ARDUINO_ARCH_ESP32 || ARDUINO_ARCH_ESP8266
    
    ESP.restart();
    
    #endif

    return 0;
}

/**
 * @brief To to print the help text.
 */
CLI_COMMAND(help)
{
    Serial.printf("Supported commands:\n");
    Serial.printf("  ver         Used to print version infos.\n");
    Serial.printf("  led mode    Used to control the led. \n");
    Serial.printf("                Supported modes:\n");
    Serial.printf("                0 ... turns the led off.\n");
    Serial.printf("                1 ... turns the led on.\n");
    Serial.printf("                b ... let it blink.\n");
    Serial.printf("  cfg         Used to print the tty configuration.\n");
    Serial.printf("  err ret     Used to test errors in a command.\n");
    Serial.printf("                ret   return value of the called function.\n");
    Serial.printf("  list [args] Used to test how arguments are parsed.\n");
    Serial.printf("                args  a list of arguments.\n");
    Serial.printf("  bell        Used to ring the bell of the host terminal.\n");
    Serial.printf("  echo on|off Used to turn echo on or off.\n");
    Serial.printf("  reset       Used to reset the CPU.\n");
    Serial.printf("  help        Prints this text.\n");

    return 0;
}

void setup()
{
    pinMode(LED_BUILTIN, OUTPUT);
 
    Serial.begin(115200);
    while (!Serial);   

    Serial.println();
    cmd_ver(0, 0);
    cli.begin();
}

void loop()
{
    uint32_t now = millis();

    if (ledTask.isScheduled(now))
    {
        if (ledMode == LED_BLINK)
        {
            digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
        }
    }

    cli.loop();
}