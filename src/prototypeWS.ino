/*
  ESP8266 WebSocket conection
  Use of the wifimanager to AP library
*/
/*
   Copyright 2017 Damian Nogueiras
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

#include "Config.h"
#include "WifiConnection.h"
#include "Radio.h"

// Update these with values suitable for your network.
const char* ssid = "wifi";
const char* password = "perlitalagatita";

// wifi Configuration
WifiConnection wifiRobota;
// connection with websocket
Radio radio;

void setup () {
  // init de serial comunication
  Serial.begin(115200);
  //built in led and blue led
  pinMode(LED_BUILTIN,OUTPUT);
  pinMode(D4,OUTPUT);
  // only AP
  //id_connected = WEBLOCAL;
  // wifi with internet -> MQTT
  id_connected = WIFIMQTT;

  switch (id_connected) {
    case WIFIMQTT:
      setup_wifi();
      //wifiRobota.MQTT();
      radio.init();
      break;
    case NONET:
      break;
    case WEBLOCAL:
      wifiRobota.onlyAP();
      // init websocket server or TODO: MQTT
      radio.init();
      break;
  }
}

long lastMsg = millis();
int value = 0;

void loop() {
  String msg;
  radio.wsloop();
  long now = millis();
  if (now - lastMsg > 5000) {
    lastMsg = now;
    ++value;
    msg = "ON ";
    msg += value;
    // keep alive, server to everybody
    radio.wsbroadcast(msg);
  }
}

void setup_wifi() {

  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  randomSeed(micros());

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}
