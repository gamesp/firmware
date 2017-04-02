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

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266mDNS.h>
#include <WiFiManager.h>

#include <ArduinoJson.h>
#include <Hash.h>

#include "Config.h"
#include "Motion.h"
#include "Radio.h"

WiFiManager wifiManager;

// connection with websocket
Radio radio;
// Motors of robota to mov
Motion motors;

void configModeCallback(WiFiManager *myWiFiManager){
  if (DEBUG) {
    Serial.println("Entered config mode");
    Serial.println(WiFi.softAPIP());
    //if you used auto generated SSID, print it
    Serial.println(myWiFiManager->getConfigPortalSSID());
  }
}

void setup () {
  // init de serial comunication
  Serial.begin(115200);
  // in the begining there isn't connection
  id_connected = NONET;
  // reset param connection Esp8266 only for test
  wifiManager.resetSettings();
  // try to connect
  wifiManager.setAPCallback(configModeCallback);
  // only wifi with high signal
  wifiManager.setMinimumSignalQuality(50);
  // sets timeout until configuration portal gets turned off
  // and active de web server
  wifiManager.setTimeout(10);

  if(!wifiManager.autoConnect("WifiDefault")) {
    // if no wifi config
    Serial.println("Failed to connect and hit timeout, Mode: WEBLOCAL");
    id_connected = WEBLOCAL;
  }
  Serial.print("Mode: ");
  Serial.println(id_connected);
  Serial.print("Ip: ");
  Serial.println(WiFi.localIP());
  switch (id_connected) {
    case MQTT:
      break;
    case NONET:
      break;
    case WEBLOCAL:
      WiFi.mode(WIFI_AP);
      WiFi.softAP("AlfaRobota");
      WiFi.softAPConfig(apIP, apIP, netMsk);
      delay(100);
      // init websocket server
      radio.init();
      // Add service to MDNS
      MDNS.addService("http", "tcp", 80);
      MDNS.addService("ws", "tcp", 81);
      Serial.println("HTTP and WebSocket servers started");
      Serial.print("Ip: ");
      Serial.println(WiFi.softAPIP());
      break;
  }
}

long lastMsg = millis();
int value = 0;

void loop() {
    String msg, JSONtoString;
    radio.wsloop();

    long now = millis();
    if (now - lastMsg > 5000) {
      // Memory pool for JSON object tree.
      StaticJsonBuffer<200> keepjsonBuffer;
      JsonObject& keepJSON = keepjsonBuffer.createObject();
      lastMsg = now;
      ++value;
      msg = "ON ";
      msg += value;
      keepJSON["idRobota"] = idRobota;
      keepJSON["state"] = msg;
      // convert object JSON to string
      keepJSON.printTo(JSONtoString);
      Serial.println(JSONtoString);
      // keep alive, server to everybody
      radio.wsbroadcast(JSONtoString);
      //reset because priontTo add
      JSONtoString="";
    }
}
