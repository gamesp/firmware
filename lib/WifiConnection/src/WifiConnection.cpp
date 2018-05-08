/*
Copyright (C) 2017 Damian Nogueiras - Gamesp
This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.
This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.
You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301,
USA.
See LICENSE.txt for details
*/

#include <Arduino.h>
#include "WifiConnection.h"

/**
 * Constructor
 */
WifiConnection::WifiConnection() {
    WiFi.mode(WIFI_OFF);
    WiFi.mode(WIFI_AP_STA);
    // reset ssid saved
    WiFi.disconnect();
    // take mac address
    WiFi.macAddress(_mac);
    // append the two final hex of mac
    _NameString += "-" + String(_mac[4],HEX) + String(_mac[5],HEX);
    // convert string to const char *
    _AP_NameString = _NameString.c_str();
    // up Access Point
    modeAP();
  };
  /**
   * Wifi setup
   * @return true if wifi connected, false if not
   */
bool WifiConnection::wifiSetup(const char * ssid, const char * pass){
    Serial.println("\n\tConnecting to Wi-Fi");
    Serial.println(ssid);
    Serial.println(pass);
    WiFi.disconnect();
    WiFi.begin(ssid, pass);
    int connRes = WiFi.waitForConnectResult();
    if (connRes != WL_CONNECTED) {
        Serial.println("[WiFi] Connection failed");
        Serial.println(connectionStatus( WiFi.status() ).c_str() );
        WiFi.disconnect();
        modeAP();
        return false;
    } else {
        Serial.println("[WiFi] Connected");
        return true;
    }
}

bool WifiConnection::wifiDebug(){
    int connRes = WiFi.waitForConnectResult();
    if (connRes != WL_CONNECTED) {
        if (DEBUG_W) {
            Serial.print("[WiFi debug] ");
            Serial.println(connectionStatus( WiFi.status() ).c_str() );
        }
        return false;
    } else {
        if (DEBUG_W) Serial.println("[WiFi debug] Connected");
        return true;
    }
}

/********************************************************
/*  WiFi Connection Status                              *
/********************************************************/
String WifiConnection::connectionStatus ( int which )
{
    switch ( which )
    {
        case WL_CONNECTED:
            return "Connected";
            break;

        case WL_NO_SSID_AVAIL:
            return "Network not availible";
            break;

        case WL_CONNECT_FAILED:
            return "Wrong password";
            break;

        case WL_IDLE_STATUS:
            return "Idle status";
            break;

        case WL_DISCONNECTED:
            return "Disconnected";
            break;

        default:
            return "Unknown";
            break;
    }
}
/**
 * Mode AP
 */
void WifiConnection::modeAP() {
  Serial.print("AP:");
  Serial.println(_AP_NameString);
  // ssid, passwd (null for open wifi), channel, ssid_hidden (0 broadcast, 1 hidden), max_connection
  // only one client, only one connection
  WiFi.softAP(_AP_NameString, NULL, 1, 0, 1);
  Serial.println("Access Point started");
  Serial.print("Ip: ");
  Serial.println(WiFi.softAPIP());
  delay(200);
}
String WifiConnection::getSSID() {
  return _NameString;
}
const char * WifiConnection::getAP() {
  return _AP_NameString;
}
