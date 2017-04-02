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

#include <WebSocketsServer.h>
#include <ArduinoJson.h>
#include <Hash.h>
#include <ESP8266WebServer.h>

#include "Motion.h"

// DEBUG true (1) or false (0)
#define DEBUG 1
#define USE_SERIAL Serial

// id_connected
#define NONET     0  // no connection
#define MQTT      1  // there is wifi and mqtt broker connection
#define WEBLOCAL  2  // no wifi or no mqtt broker connection
uint8_t id_connected;

WiFiManager wifiManager;

IPAddress apIP(192, 168, 4, 1);
IPAddress netMsk(255, 255, 255, 0);
// TODO concat a chip reference
String idRobota = "alfarobota";

// listen webserver at port 80
ESP8266WebServer server = ESP8266WebServer(80);
// listen websocket at port 81
WebSocketsServer webSocket = WebSocketsServer(81);

// Motors of robota to mov
Motion motors;

//manage event
void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t lenght) {

    String stringWS;
    switch(type) {
        case WStype_DISCONNECTED:
            Serial.printf("[%u] Disconnected!\n", num);
            break;
        case WStype_CONNECTED: {
            // Memory pool for JSON object tree.
            StaticJsonBuffer<200> jsonBuffer;
            IPAddress ip = webSocket.remoteIP(num);
            Serial.printf("[%u] Connected from %d.%d.%d.%d url: %s\n", num, ip[0], ip[1], ip[2], ip[3], payload);
            JsonObject& objectWS = jsonBuffer.createObject();
            objectWS["id"] = idRobota;
            objectWS["state"] = "Client connected";
            objectWS.printTo(stringWS);
            // send message to client
            webSocket.sendTXT(num, stringWS);
            stringWS = "";
        }
            break;
        case WStype_TEXT:
            // Memory pool for JSON object tree.
            StaticJsonBuffer<200> rxjsonBuffer;
            // counter for
            int i;
            Serial.println();
            Serial.printf("[%u]: %s\n", num, payload);
            // convert payload to JSON object
            // http://arduino.stackexchange.com/questions/30209/cast-from-uint8-t-to-char-loses-precision
            JsonObject& rxWS = rxjsonBuffer.parseObject((char *)payload);
            if (!rxWS.success()) {
              Serial.println("parseObject() failed");
              return;
            }
            // TODO comprobar que llega commands
            const char* commands = rxWS["commands"];
            Serial.println(commands);
             for (i = 0; i < strlen((const char *)(commands)); i++) {
               Serial.println((char)commands[i]);
               // action for different commands
               switch ((char)commands[i]) {
                 case 'F':
                  motors.movForward(1);
                  break;
                case 'R':
                  motors.turnRight();
                  break;
                case 'L':
                  motors.turnLeft();
                  break;
                case 'B':
                  motors.movBack(1);
                  break;
                // stop
                case 'S':
                  motors.stop();
                  break;
              }
            } // loop evry commands
            // stop robota anyway after executing
            motors.stop();
            break;
    } // switch type of WS
  } // webSocketEvent


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
      // start webSocket server
      webSocket.begin();
      webSocket.onEvent(webSocketEvent);
      // start webServer
      server.on("/", HTTP_GET, [](){
      server.send(200, "text/html", "<h1>ESP8266 Web Server</h1>");
        });
      server.begin();
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
    webSocket.loop();
    server.handleClient();
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
      webSocket.broadcastTXT(JSONtoString);
      //reset because priontTo add
      JSONtoString="";
    }
}
