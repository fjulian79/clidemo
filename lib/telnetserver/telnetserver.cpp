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

#include "telnetserver.hpp"

/**
 * Currently the TelnetServer is only supported on ESP32 platforms.
 */
#if HAS_WIFI_SUPPORT

#include <WiFi.h>
#include <cli/cli.hpp>

/**
 * Defining those instances here avoids the need of having them as member of 
 * the class. This is a bit dirty, but allows a generic class which can be on 
 * any platform.
 */
namespace tsrvGlobal
{
    WiFiServer telnetServer(23);
    WiFiClient telnetClient;
    WiFiClient wifiClient;
    Cli telnetCli;
}

void TelnetServer::wifiSetup(char* ssid, char* passwd)
{
    strncpy(this->ssid, ssid, sizeof(this->ssid));
    strncpy(this->passwd, passwd, sizeof(this->passwd));
}

int8_t TelnetServer::begin(void)
{
    const uint8_t timeout_sec = 5;
    uint32_t start = 0;
    uint32_t ms = 0;

    if(strlen(ssid) == 0 || strlen(passwd) == 0)
    {
        Serial.println("WiFi: No SSID or password set.");
        return -1;
    }

    Serial.printf("WiFi: Connecting to %s, ", ssid);
    WiFi.mode(WIFI_STA);

    start = millis(); 
    WiFi.begin(ssid, passwd);

    while (WiFi.status() != WL_CONNECTED)
    {
        ms = millis() - start;
        if(ms > (timeout_sec * 1000))
        {
            Serial.printf("Timeout(%u sec)\n", timeout_sec);
            return -1;
        }
    }

    ms = millis() - start;
    Serial.printf("%s (%ums)\n", WiFi.localIP().toString().c_str(), ms);
    randomSeed(micros()); 
    
    tsrvGlobal::telnetServer.begin();
    tsrvGlobal::telnetServer.setNoDelay(true);
    tsrvGlobal::telnetCli.setEcho(false);
    Serial.println("Telnet-Server started");

    return 0;   
}

bool TelnetServer::wifiConnected(void)
{
    return WiFi.status() == WL_CONNECTED;
}

bool TelnetServer::clientConnected(void)
{
    return tsrvGlobal::telnetClient.connected();
}

void TelnetServer::info(Stream &ioStream)
{
    ioStream.println("Telnet-Server:");
    ioStream.printf("  MAC:           %s\n", WiFi.macAddress().c_str());
    ioStream.printf("  WiFi Status:   %s\n", WiFi.isConnected() ? "Connected" : "Connecting ...");
    ioStream.printf("  WiFi IP:       %s\n", WiFi.localIP().toString().c_str());
    ioStream.printf("  WiFi RSSI:     %d\n", WiFi.RSSI());
    ioStream.printf("  Telnet-Client: %s\n", tsrvGlobal::telnetClient.connected() ? "Connected" : "Disconnected");
}

void TelnetServer::loop(void)
{
    if (tsrvGlobal::telnetServer.hasClient()) 
    {
        if (state == idle)
        {
            tsrvGlobal::telnetClient = 
                tsrvGlobal::telnetServer.available();
            Serial.printf("Telnet-Client %s connected.\n", 
                tsrvGlobal::telnetClient.remoteIP().toString().c_str());
            state = connecting;
        }
        else
        {
            WiFiClient newClient = tsrvGlobal::telnetServer.available();
            Serial.printf("Telnet-Client %s connected, rejecting %s.\n", 
                tsrvGlobal::telnetClient.remoteIP().toString().c_str(),
                newClient.remoteIP().toString().c_str());
            newClient.stop();
        }
    }

    if(state == connecting)
    {
        tsrvGlobal::telnetClient.print("\033c");
        CliCommand::exec(tsrvGlobal::telnetClient, "ver", 0, 0);
        tsrvGlobal::telnetClient.printf("Info: This telent session operates in Line Mode.\n");
        tsrvGlobal::telnetClient.printf("      Input is processed upon pressing Enter.\n");
        tsrvGlobal::telnetClient.printf("\n");
        tsrvGlobal::telnetClient.printf("Use the 'help' command to get a list of available commands.\n\n");
        tsrvGlobal::telnetCli.begin(&tsrvGlobal::telnetClient);
        state = connected;
    }

    if(state == connected && tsrvGlobal::telnetClient.connected())  
    {
        tsrvGlobal::telnetCli.loop();
    }

    if (state == connected && !tsrvGlobal::telnetClient.connected())
    {
        Serial.printf("Telnet-Client %s disconnected.\n", 
            tsrvGlobal::telnetClient.remoteIP().toString().c_str());
            tsrvGlobal::telnetClient.stop();
        state = idle;
    }
}

#else

void TelnetServer::wifiSetup(char* ssid, char* passwd)
{
    // nothing to do
}   

int8_t TelnetServer::begin(void)
{
    info(Serial);
    return -1;
}

bool TelnetServer::wifiConnected(void)
{
    return false;
}

bool TelnetServer::clientConnected(void)
{
    return false;
}

void info(Stream &ioStream)
{
    Serial.println("Telnet-Server not supported on this platform.");
}

void TelnetServer::loop(void)
{
    // nothing to do
}

#endif /* ARDUINO_ARCH_ESP32 */
