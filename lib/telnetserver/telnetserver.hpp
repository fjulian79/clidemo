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

 #ifndef _NETWORKING_HPP_
#define _NETWORKING_HPP_

#include <Arduino.h>

/**
 * @brief Used as central place to check if the platform has WiFi support.
 */
#ifdef ARDUINO_ARCH_ESP32

#define HAS_WIFI_SUPPORT    1

#endif

/**
 * @brief This class provides a simple telnet server.
 * On platforms without WiFi support this class will do nothing.
 */
class TelnetServer
{
    public:

        /**
         * @brief Constructor does nothing special.
         */
        TelnetServer(void)
        {
            memset(ssid, 0, sizeof(ssid));
            memset(passwd, 0, sizeof(passwd));
            state = idle;
        }
        
        /**
         * @brief Sets the WiFi credentials.
         * @param ssid   The SSID of the WiFi network.
         * @param passwd The password of the WiFi network.
         */
        void wifiSetup(char* ssid, char* passwd);
        
        /**
         * @brief Starts the telnet server.
         * @return 0 on success, -1 on error.
         */
        int8_t begin(void);

        /**
         * @brief Tells if the WiFi connection is established.
         */
        bool wifiConnected(void);

        /**
         * @brief Tells if a client is connected.
         */
        bool clientConnected(void);
        
        /**
         * @brief Prints wifi and server infos to the given stream.
         */
        void info(Stream &ioStream = Serial);
    
        /**
         * @brief This function must be called in the loop() function.
         */
        void loop(void);

    private:

        /**
         * @brief The SSID of the WiFi network.
         */
        char ssid[32];

        /**
         * @brief The password of the WiFi network.
         */
        char passwd[32];
        
        /**
         * @brief Internal state of the telnet server.
         */
        enum {idle = 0, connecting, connected} state;
};

#endif /* _NETWORKING_HPP_ */