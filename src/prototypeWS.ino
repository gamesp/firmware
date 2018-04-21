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
        delay(1000);
    }
  //built in led and blue led
  pinMode(LED_BUILTIN,OUTPUT);
  pinMode(D4,OUTPUT);

  // init websocket server TODO and MQTT if wifi connected
  radio.init();
}

long lastMsg = millis();
int value = 0;

void loop() {
  String msg;
  radio.loop();
  long now = millis();
  if (now - lastMsg > 5000) {
    lastMsg = now;
    ++value;
    msg = "ON ";
    msg += value;
    if (radio._isWIFI) msg+=",wifi ok";
    if (radio._isMQTT) msg+=",mqtt ok";
    // keep alive, server to everybody
    radio.broadcast(msg);
    /*
    if (WiFi.status() != WL_CONNECTED) {
      if (DEBUG_W) Serial.println("No WIFI");
  }*/
  }

}
