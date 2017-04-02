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

#include "Radio.h"
#include <Arduino.h>
#include <ArduinoJson.h>

/**
 * init websocket server
 */
void Radio::init() {
  // start webSocket server
  webSocket.begin();
  // to define event use lambda and references
  // http://stackoverflow.com/questions/39803135/c-unresolved-overloaded-function-type
  // http://stackoverflow.com/questions/4940259/lambdas-require-capturing-this-to-call-static-member-function
  webSocket.onEvent([&](uint8_t num, WStype_t type, uint8_t * payload, size_t lenght){

    String stringWS;
    switch(type) {
        case WStype_DISCONNECTED:
            Serial.printf("[%u] Disconnected!\n", num);
            break;
        case WStype_CONNECTED: {
            // Memory pool for JSON object tree.
            StaticJsonBuffer<200> jsonBuffer;
            Serial.printf("[%u] Connected client\n", num);
            JsonObject& objectWS = jsonBuffer.createObject();
            objectWS["id"] = "alfarobota";
            objectWS["state"] = "Client connected";
            objectWS.printTo(stringWS);
            // send message to client
            Radio::wssend(num, stringWS);
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
        /*     for (i = 0; i < strlen((const char *)(commands)); i++) {
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
            motors.stop();*/
            break;
    } // switch type of WS


  });
}
/**
 * loop websocket server
 */
 void Radio::wsloop() {
   webSocket.loop();
 }
 /**
  *  send a broadcast
  */
void Radio::wsbroadcast(String msg){
  // TODO format JSON msg
  webSocket.broadcastTXT(msg);
}
/**
 *  send a simple text
 */
void Radio::wssend(uint8_t num, String msg){
  webSocket.sendTXT(num,msg);
}
