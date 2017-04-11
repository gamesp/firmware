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
void WifiConnection::onlyAP() {
  if (DEBUG) {
    Serial.print("AP:");
    Serial.println(_AP_NameString);
  }
  WiFi.softAP(_AP_NameString);
  delay(100);
}
