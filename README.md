# clidemo

A demonstration and test bench for the `libcli` library, showcasing its capabilities in embedded projects.

## Features

- **Command-Line Interface**: Demonstrates how to implement a responsive CLI using `libcli`.
- **Command Registration**: Showcases automatic command registration via the `CLI_COMMAND(name)` macro.
- **Stream-Based Transport**: Utilizes serial communication to interact with the CLI.
- **Optional Telnet Support**: On ESP32, a telnet server can be started.
- **VT100 Terminal Support**: Implements selected VT100 sequences for enhanced terminal usability.
- **Unit Testing**: Includes a set of unit tests to validate the functionality of `libcli`.

## Changelog

See the [CHANGELOG](CHANGELOG.md) for a detailed list of changes and updates.

## Installation

### Prerequisites
- Ensure you have PlatformIO installed.
- Clone the `clidemo` repository to your local machine.

### Steps
1. **Clone the Repository**:
   ```bash
   git clone https://github.com/fjulian79/clidemo.git
   cd clidemo
   ```
2. **Build the Project**:
   Use PlatformIO to build the project:
   ```bash
   pio run
   ```

3. **Upload to Your Device**:
   Connect your embedded device and upload the firmware:
   ```bash
   pio run --target upload
   ```

4. **Open a Serial Monitor**:
   Start a serial connection to interact with the CLI:
   ```bash
   pio device monitor

5. **Run Unit Tests** (Optional):
   To run the included unit tests, execute:
   ```bash
   unittest all
   ```

## Usage
Once connected via serial, you can type commands to interact with the system. 
```
Please build project in debug configuration to get more details about an exception.
See https://docs.platformio.org/page/projectconf/build_configurations.html


--- Terminal on /dev/cu.usbmodem201101 | 115200 8-N-1
--- Available filters and text transformations: debug, default, direct, hexlify, log2file, nocontrol, printable, send_on_enter, time
--- More details at https://bit.ly/pio-monitor-filters
--- Quit: Ctrl+C | Menu: Ctrl+T | Help: Ctrl+T followed by Ctrl+H
Git Repo: https://github.com/fjulian79/clidemo.git
Revision: 9f639c8-dirty

This program comes with ABSOLUTELY NO WARRANTY. This is free software, and you
are welcome to redistribute it under certain conditions.
See GPL v3 licence at https://www.gnu.org/licenses/ for details.

Use the 'help' command to get a list of available commands.

#>
#>
#>help
Supported commands:
  ver             Used to print version infos.
  led 0|1|b       Used to control the led. 
                    0   turns the led off.
                    1   turns the led on.
                    b   let it blink.
  telnet cmd      Used to control the telnet server.
                    begin ssid passwd
                    info
  info            Used to print lib cli infos.
  unittest <test> Used to run unit tests.
                    Use argument 'all' to run all tests.
  err ret         Used to test errors in a command.
                    ret   return value of the command.
  list [args]     Used to test how arguments are parsed.
  bell            Used to ring the bell of the host terminal.
  echo on|off     Used to turn echo on or off.
  reset           Used to reset the CPU.
  help            Prints this text.

#>
#>info

Lib Cli Infos:
  Version:               v4.3.1
  CLI_COMMANDSIZ:        100
  CLI_HISTORYSIZ:        200
  CLI_ARGVSIZ:           4
  CLI_PROMPT:            #>
  CLI_BUFFEREDIO:        0
  SERIAL_RX_BUFFER_SIZE: 0
  Supported commands:    15
  Registered commands:   12
  Dropped commands:      0
  Registered Command's:
    ver
    led
    info
    err
    args
    echo
    bell
    reset
    telnet
    help
    dummy
    unittest

#>
```

## License
This library is licensed under GPL-3.0.

## Author
Developed by **Julian Friedrich**. For contributions or issues, visit the [GitHub repository](https://github.com/fjulian79/clidemo).

## Contributions
Contributions are welcome! Feel free to submit issues or pull requests to improve the demo and enhance `libcli` usability.

---
> **Note:** This project is intended as a demonstration and testbench of `libcli` and may not be suitable for production use without modifications.

