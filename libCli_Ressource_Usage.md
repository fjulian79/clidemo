# Impact of libCli features on RAM and Flash usage

This file shows the resource impact of libCli features on RAM and Flash usage for various platforms. The values are based on the CLI Demo project in this repository.

**IMPORTANT:** Some cliDome code depends on libCLI Features and may compromise the measurement. Therefore 
```RESOURCE_USAGE_TEST``` is prepared in platformio.ini to disable some code parts for more accurate measurements. If you want to test the resource usage, please enable ```RESOURCE_USAGE_TEST``` and adjust the ```CLI_HISTORYSIZ``` and ```CLI_TAB_COMPLETION``` values as needed.

## nodemcu-32s (ESP32) Measurment
- libCli v4.6.0
- Date: 2026-04-11
````
-D CLI_COMMANDS_MAX=25
-D CLI_HISTORYSIZ=0
-D CLI_TAB_COMPLETION=0
RAM:   [=         ]   6.7% (used 21960 bytes from 327680 bytes)
Flash: [==        ]  21.2% (used 278465 bytes from 1310720 bytes)

-D CLI_COMMANDS_MAX=25
-D CLI_HISTORYSIZ=200
-D CLI_TAB_COMPLETION=0
RAM:   [=         ]   6.8% (used 22184 bytes from 327680 bytes)
Flash: [==        ]  21.3% (used 279457 bytes from 1310720 bytes)

-D CLI_COMMANDS_MAX=25
-D CLI_HISTORYSIZ=200
-D CLI_TAB_COMPLETION=0
RAM:   [=         ]   6.8% (used 22184 bytes from 327680 bytes)
Flash: [==        ]  21.4% (used 280121 bytes from 1310720 bytes)
````
### Summary
- **History (200 entries)**: +224 bytes RAM, +992 bytes Flash
- **Tab Completion**: 0 bytes RAM, +664 bytes Flash

## pico (RP2040) Measurement
- libCli v4.6.0
- Date: 2026-04-11

````
-D CLI_COMMANDS_MAX=25
-D CLI_HISTORYSIZ=0
-D CLI_TAB_COMPLETION=0
RAM:   [          ]   3.5% (used 9228 bytes from 262144 bytes)
Flash: [          ]   3.7% (used 76508 bytes from 2093056 bytes)

-D CLI_COMMANDS_MAX=25
-D CLI_HISTORYSIZ=200
-D CLI_TAB_COMPLETION=0
RAM:   [          ]   3.6% (used 9452 bytes from 262144 bytes)
Flash: [          ]   3.7% (used 77324 bytes from 2093056 bytes)

-D CLI_COMMANDS_MAX=25
-D CLI_HISTORYSIZ=200
-D CLI_TAB_COMPLETION=1
RAM:   [          ]   3.6% (used 9452 bytes from 262144 bytes)
Flash: [          ]   3.7% (used 78036 bytes from 2093056 bytes)
````

### Summary
- **History (200 entries)**: +224 bytes RAM, +816 bytes Flash
- **Tab Completion**: 0 bytes RAM, +712 bytes Flash

