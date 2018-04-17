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

#include <ESP8266WiFi.h>

#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>

#include "Config.h"
#include "WifiConnection.h"
#include "Radio.h"
// ssid and password
#include "wifiparam.h"

//WiFiManager Init
WiFiManager wifiManager;
// wifi Configuration
WifiConnection wifiRobota;
// connection
Radio radio;

// if exist wifi then connect to broker
bool isMQTT = false;

void configModeCallback(WiFiManager *myWiFiManager){
    Serial.println("(CallBack)Entered AP only mode");
    Serial.println(WiFi.softAPIP());
    //if you used auto generated SSID, print it
    Serial.println(myWiFiManager->getConfigPortalSSID());
}

void setup () {
  // init de serial comunication
  Serial.begin(115200);
  //built in led and blue led
  pinMode(LED_BUILTIN,OUTPUT);
  pinMode(D4,OUTPUT);

  // WiFiManager setup
  // Uncomment and run it once, if you want to erase all the stored information
  wifiManager.resetSettings();
  //set callback that gets called when connecting to previous WiFi fails, and enters Access Point mode
  wifiManager.setAPCallback(configModeCallback);
  //sets timeout until configuration portal gets turned off
  //useful to make it all retry or go to sleep
  //in seconds
  wifiManager.setTimeout(30);
  //fetches ssid and pass and tries to connect
  //if it does not connect it starts an access point with the specified name
  //here  "DOMOTTA-XXXX"
  //and goes into a blocking loop awaiting configuration
  if( !wifiManager.autoConnect(wifiRobota.getAP()) ) {
    Serial.println("failed to connect and hit timeout");
  } else {
    Serial.println("WiFi connected!!!");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
  }
  wifiRobota.onlyAP();

  // init websocket server TODO and MQTT if wifi connected
  radio.init(isMQTT);
}

long lastMsg = millis();
int value = 0;

void loop() {
  String msg;
  radio.loop(isMQTT);
  long now = millis();
  if (now - lastMsg > 5000) {
    lastMsg = now;
    ++value;
    msg = "ON ";
    msg += value;
    // keep alive, server to everybody
    radio.broadcast(msg, isMQTT);
    /*
    if (WiFi.status() != WL_CONNECTED) {
      if (DEBUG_W) Serial.println("No WIFI");
  }*/
  }

}
