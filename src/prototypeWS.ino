/*
  Firmware, Gamesp
*/
/*
   Copyright 2017 Gamesp - Damian Nogueiras
   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
   GNU General Public License for more details.
   You should have received a copy of the GNU General Public License
   along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

#include <Arduino.h>
#include "Radio.h"

// connection
Radio radio;

void setup () {
  // init de serial comunication
  Serial.begin(115200);
  Serial.setDebugOutput(true);
  Serial.println();
  Serial.println();
  Serial.println();

    for(uint8_t t = 4; t > 0; t--) {
        Serial.printf("[SETUP] BOOT WAIT %d...\n", t);
        Serial.flush();
        delay(100);
    }
  //built in led and blue led
  pinMode(LED_BUILTIN,OUTPUT);
  pinMode(D4,OUTPUT);

  // init websocket server and MQTT if wifi connected
  radio.init();
}

void loop() {
  // websocket and mqtt loop
  radio.loop();
}
