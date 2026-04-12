/*
 * clidemo, a example and test bench for my command line library libcli.
 *
 * Copyright (C) 2026 Julian Friedrich
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
#include "telnetserver.hpp"

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
 * @brief Used to define dummy commands for testing the command listing and
 * command completion functionality.
 * @arg   _name  The name of the dummy command, e.g. "foo"
 */
#define DUMMY_CMD(_name)                                            \
    CLI_COMMAND(_name) {                                            \
        ioStream.printf("Executed dummy command %s\n", #_name);     \
        return 0;                                                   \
    }

/**
 * @brief Defines the operational mode of the led.
 */
typedef enum{
	LED_STATIC = 0,
	LED_BLINK
}led_mode_t;

/**
 * @brief The global command line interface instance.
 */
Cli cli;

/**
 * @brief The global telnet server instance.
 */
TelnetServer telnetServer;

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
CLI_COMMAND(ver) {
    ioStream.printf("\n%s %s, Copyright (C) 2025 Julian Friedrich\n",
            VERSION_PROJECT, VERSION_GIT_SHORT);
    ioStream.printf("Build:           %s, %s\n", __DATE__, __TIME__);
    ioStream.printf("Git Repo:        %s\n", VERSION_GIT_REMOTE_ORIGIN);
    ioStream.printf("Revision:        %s\n", VERSION_GIT_LONG);
    ioStream.printf("libCli Version:  %s\n", CLI_VERSION);
    ioStream.printf("\n");
    ioStream.printf("This program comes with ABSOLUTELY NO WARRANTY. This is free software, \n");
    ioStream.printf("and you are welcome to redistribute it under certain conditions.\n");
    ioStream.printf("See GPL v3 licence at https://www.gnu.org/licenses/ for details.\n\n");

    return 0;
}

/**
 * @brief Used to control the on board led
 * @arg   mode  0|1|b
 */
CLI_COMMAND(led) {
	if (argc != 1) {
		return -1;
	}

	if (*argv[0] == '0') {
		ledMode = LED_STATIC;
		digitalWrite(LED_BUILTIN, 0);
	} else if (*argv[0] == '1') {
		ledMode = LED_STATIC;
		digitalWrite(LED_BUILTIN, 1);
	} else if (*argv[0] == 'b') {
		ledMode = LED_BLINK;
	}

	return 0;
}

/**
 * @brief Turns the LED on.
 * Does the same as "led 1", but is used to test command completion and command 
 * listing.
 */
CLI_COMMAND(led_on){
    ledMode = LED_STATIC;
    digitalWrite(LED_BUILTIN, 1);
    return 0;
}

/**
 * @brief Turns the LED off.
 * Does the same as "led 0", but is used to test command completion and command
 * listing.
 */
CLI_COMMAND(led_off){
    ledMode = LED_STATIC;
    digitalWrite(LED_BUILTIN, 0);
    return 0;
}

/**
 * @brief Makes the LED blink.
 * Does the same as "led b", but is used to test command completion and command
 * listing.
 */
CLI_COMMAND(led_blink){
    ledMode = LED_BLINK;
    return 0;
}

/**
 * @brief Used to list all registered commands.
 * This command is used to test the command listing functionality and to check 
 * if all commands are properly registered. Can also be used to check completion
 * as it shares the leading l with the led command and its friends.
 */
CLI_COMMAND(list) {
    cliCmd_t *pCmdTab = CliCommand::getTable();
    size_t cmdCnt = CliCommand::getCmdCnt();

    ioStream.printf("Registered Command's:\n");

    for(size_t i = 0; i < cmdCnt; i++){
        ioStream.printf("  %s\n", pCmdTab[i].name);
    }
    ioStream.print("\n");

    return 0;
}

/**
 * @brief Prints infos on lib cli
 */
CLI_COMMAND(info) {
    #if HAS_WIFI_SUPPORT
    telnetServer.info(ioStream);
    #endif

    ioStream.printf("\nLib Cli Infos:\n");
    ioStream.printf("  libcli Version:              %s\n", CLI_VERSION);
    ioStream.printf("  CLI_COMMANDSIZ:              %d\n", CLI_COMMANDSIZ);
    ioStream.printf("  CLI_HISTORYSIZ:              %d\n", CLI_HISTORYSIZ);
    ioStream.printf("  CLI_ARGVSIZ:                 %d\n", CLI_ARGVSIZ);
    ioStream.printf("  CLI_PROMPT:                  %s\n", CLI_PROMPT);
    ioStream.printf("  CLI_BUFFEREDIO:              %d\n", CLI_BUFFEREDIO);
    ioStream.printf("  CLI_TAB_COMPLETION:          %d\n", CLI_TAB_COMPLETION);
    ioStream.printf("  CLI_TERMINAL_WIDTH:          %d\n", CLI_TERMINAL_WIDTH);
    ioStream.printf("  CLI_CMDTAB_SORTING_DEFAULT:  %d\n", CLI_CMDTAB_SORTING_DEFAULT);
    ioStream.printf("  SERIAL_RX_BUFFER_SIZE:       %d\n", SERIAL_RX_BUFFER_SIZE);
    ioStream.printf("  Supported commands:          %d\n", CLI_COMMANDS_MAX);
    ioStream.printf("  Registered commands:         %zu\n", CliCommand::getCmdCnt());
    ioStream.printf("  Dropped commands:            %zu\n", CliCommand::getDropCnt());
    ioStream.printf("\n");

    CliCommand::exec(ioStream, "list", nullptr, 0);

    return 0;
}

/**
 * @brief Used to test errors in a command.
 * @arg   ret return value of the command
 */
CLI_COMMAND(err) {
    int16_t val = -1;

    if (argc > 0) {
        val = strtol(argv[0],0 ,0);
        ioStream.printf("Got value %d\n", val);
    }

    return (int8_t) val;
}

/**
 * @brief Prints all provided arguments.
 * @arg   [args] Optional list of arguments.
 */
CLI_COMMAND(args) {
    ioStream.printf("Recognized arguments:\n");
    for(size_t i = 0; i < argc; i++) {
        ioStream.printf("  argv[%zu]: \"%s\"\n", i, argv[i]);
    }

    return 0;
}

/**
 * @brief Used to change the echo mode of libcli
 * @arg   mode on|off
 */
CLI_COMMAND(echo) {
    bool state = true;

    if(argc != 1) {
        return -1;
    }

    if(strcmp(argv[0],"on") == 0) {
        state = true;
    } else if(strcmp(argv[0],"off") == 0) {
        state = false;
    } else{
        return -2;
    }

    cli.setEcho(state);
    ioStream.printf("Echo is now %s\n", argv[0]);

    return 0;
}

/**
 * @brief Rings the bell in the host terminal.
 */
CLI_COMMAND(bell) {
    cli.sendBell();
    ioStream.printf("Sent a bell cmd\n");

    return 0;
}

/**
 * @brief Trigger a CPU reset.
 */
CLI_COMMAND(reset) {
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

CLI_COMMAND(telnet) {
    if (argc == 3 && strcmp(argv[0], "begin") == 0) {
        telnetServer.wifiSetup((char*) argv[1], (char*) argv[2]);
        telnetServer.begin();
        return 0;
    }

    if(argc == 1 && strcmp(argv[0], "info") == 0) {
        telnetServer.info(ioStream);
        ioStream.printf("\n");
        return 0;
    }

    return -1;
}

/**
 * @brief Print's the help text.
 */
CLI_COMMAND(help) {
    ioStream.printf("Available commands:\n");
    ioStream.printf("  help                         Show this help\n");
    ioStream.printf("  ver                          Show version information\n");
    ioStream.printf("  info                         Show libCli configuration\n");
    ioStream.printf("  list                         List all registered commands\n");
    ioStream.printf("\nLED Control:\n");
    ioStream.printf("  led <0|1|b>                  Control LED (0=off, 1=on, b=blink)\n");
    ioStream.printf("\nNetwork:\n");
    ioStream.printf("  telnet begin <ssid> <pass>   Start telnet server on supported platforms\n");
    ioStream.printf("  telnet info                  Show telnet status on supported platforms\n");
    ioStream.printf("\nSystem:\n");
    ioStream.printf("  echo <on|off>                Toggle command echo\n");
    ioStream.printf("  reset                        Reset CPU\n");
    ioStream.printf("\nTesting/Debug:\n");
    ioStream.printf("  test <name|all>              Run unit tests\n");
    ioStream.printf("  args [...]                   Show argument parsing\n");
    ioStream.printf("  err <n>                      Test error return codes\n");
    ioStream.printf("  bell                         Ring terminal bell\n");
    ioStream.printf("\n");

    return 0;
}

/**
 * @brief Dummy commands to check the command listing and command completion 
 * functionality.
 */
DUMMY_CMD(dummy);
DUMMY_CMD(dummy_2);
DUMMY_CMD(dummy_4);
DUMMY_CMD(dummy_3);
DUMMY_CMD(dummy_1);
DUMMY_CMD(dummy_long_3);
DUMMY_CMD(dummy_long_1);
DUMMY_CMD(dummy_long_2);
DUMMY_CMD(dummy_long_4);

/**
 * To test if disabled preprocessor blocks are handled correctly.
 */
#if 0

/**
 * @brief Dummy commands to check the dropCnt.
 *
 * The number of supporded commands is set in platformio.ini. 
 * if the number of registered commands exceeds the supported number of 
 * commands, the commands will be dropped and the drop count will be increased. 
 * This is used to check if the drop count is working correctly and if the 
 * library can handle more registered commands than supported.
 */
DUMMY_CMD(dummy_extra_1);
DUMMY_CMD(dummy_extra_2);
DUMMY_CMD(dummy_extra_3);

#endif

/**
 * @brief Platform-dependent serial initialization handler
 * 
 * Handles two different serial port types:
 * - USB CDC (RP2040, ESP32-S2/C3/C6 with USB): Serial only available when USB 
 *   is connected. Uses if(Serial) check to detect connection.
 * - Hardware UART (ESP32 classic, ESP8266, STM32): Serial always available, 
 *   no connection detection needed.
 */
void handleSerial(uint32_t now) {
    static uint8_t state = 0;

#if defined(ARDUINO_ARCH_RP2040) ||                                     \
    (defined(ARDUINO_USB_CDC_ON_BOOT) && ARDUINO_USB_CDC_ON_BOOT == 1)
    /* USB CDC boards - wait for USB connection */
    if (Serial && state == 0) {
        Serial.begin(115200);
        state = 1;
        return;
    }

    if (!Serial && state != 0) {
        /* USB disconnected, reset to wait for reconnection */
        state = 0;  
    }
#else
    /* Hardware UART boards - Serial always available */
    if (state == 0) {
        Serial.begin(115200);
        state = 1;
        return;
    }
#endif

    if (state == 1) {
        Serial.println();
        cmd_ver(Serial, 0, 0);
        Serial.printf(
            "Use the 'help' command to get a list of available commands.\n\n");
        cli.begin();
        state = 2;
    }

    if (state == 2) {
        cli.loop();
    }
}

void setup() {
    pinMode(LED_BUILTIN, OUTPUT);
    
    serialTask.setTaskFunction(handleSerial);
}

void loop() {
    uint32_t now = millis();

    if(ledTask.isScheduled(now)) {
        if (ledMode == LED_BLINK) {
            digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
        }
    }

    serialTask.loop(now);
    telnetServer.loop();
}