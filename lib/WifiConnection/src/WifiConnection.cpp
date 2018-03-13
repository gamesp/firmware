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

#include "WifiConnection.h"
#include <Arduino.h>

/**
 * Constructor
 */
WifiConnection::WifiConnection() {
    // take mac address
    WiFi.macAddress(_mac);
    // append the two final hex of mac
    _NameString += "-" + String(_mac[4],HEX) + String(_mac[5],HEX);
    // convert string to const char *
    _AP_NameString = _NameString.c_str();
};
/**
 * Mode AP
 */
void WifiConnection::onlyAP() {
  if (DEBUG_W) {
    Serial.print("AP:");
    Serial.println(_AP_NameString);
  }
  // ssid, passwd (null for open wifi), channel, ssid_hidden (0 broadcast, 1 hidden), max_connection
  // only one client, only one connection
  WiFi.softAP(_AP_NameString, NULL, 1, 0, 1);
  if (DEBUG_W) {
    Serial.println("WebSocket server started");
    Serial.print("Ip: ");
    WiFi.softAPIP();
  }
  delay(100);
}
String WifiConnection::getSSID() {
  return _NameString;
}
