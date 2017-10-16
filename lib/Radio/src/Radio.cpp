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

/**
 * init websocket server
 */
void Radio::init() {
  //prueba de sonido
  multimedia.sos();
  //turn off all leds
  multimedia.turnOFF();
  //define wifi parameters
  WifiConnection wificonnection;
  _idRobota = wificonnection.getSSID();
  // stop motor reset output
  motors.stop();
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
            // send message to client
            Radio::wssend(num, "Client connected");
            multimedia.movingLEDs(CRGB::Green);
            multimedia.movingLEDs(CRGB::DarkCyan);
            multimedia.movingLEDs(CRGB::Green);
            multimedia.movingLEDs(CRGB::DarkCyan);
            delay(2000);
            multimedia.led(LED_F,OFF);
            multimedia.led(LED_B,OFF);
            multimedia.led(LED_R,OFF);
            multimedia.led(LED_L,OFF);
            multimedia.led(LED_S,OFF);
        }
            break;
        case WStype_TEXT:
            // Memory pool for JSON object tree.
            StaticJsonBuffer<200> rxjsonBuffer;
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

            int i;
             for (i = 0; i < strlen((const char *)(commands)); i++) {
               if (DEBUG) {
                 Serial.println((char)commands[i]);
               }
               // turn off the central led
               multimedia.led(LED_S, OFF);
               // action for different commands
               switch ((char)commands[i]) {
                 case 'F':
                  multimedia.led(LED_F, ON);
                  motors.movForward(1);
                  multimedia.led(LED_F, OFF);
                  break;
                case 'R':
                  multimedia.led(LED_R, ON);
                  motors.turnRight();
                  multimedia.led(LED_R, OFF);
                  break;
                case 'L':
                  multimedia.led(LED_L, ON);
                  motors.turnLeft();
                  multimedia.led(LED_L, OFF);
                  break;
                case 'B':
                  multimedia.led(LED_B, ON);
                  motors.movBack(1);
                  multimedia.led(LED_B, OFF);
                  break;
                // stop
                case 'S':
                  motors.stop();
                  multimedia.led(LED_S, ON);
                  break;
              } // switch
                wsexecuting(num,(char)commands[i],motors.getX(),motors.getY(),motors.getCardinal());
            } // loop evry commands
            // stop robota anyway after executing
            motors.stop();
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
  *  send a broadcast, before create JSON
  */
void Radio::wsbroadcast(String msg){
  multimedia.led(LED_S, ON);
  String JSONtoString;
  // JSON object
  StaticJsonBuffer<200> jsonBuffer;
  JsonObject& objectJSON = jsonBuffer.createObject();
  objectJSON["idRobota"] = _idRobota;
  objectJSON["state"] = msg;
  // convert object JSON to string
  objectJSON.printTo(JSONtoString);
  webSocket.broadcastTXT(JSONtoString);
  multimedia.led(LED_S, OFF);
}
/**
 *  send a state
 * @param num id of client
 * @param msg string with state (ON + value)
 */
void Radio::wssend(uint8_t num, String msg){
  String JSONtoString;
  // JSON object
  StaticJsonBuffer<200> jsonBuffer;
  JsonObject& objectJSON = jsonBuffer.createObject();
  objectJSON["idRobota"] = _idRobota;
  objectJSON["state"] = msg;
  // convert object JSON to string
  objectJSON.printTo(JSONtoString);
  webSocket.sendTXT(num,JSONtoString);
}
/**
 *  send a executing message
 */
void Radio::wsexecuting(uint8_t num, char command, int X, int Y, char compass){
  if (DEBUG) {
    Serial.print("Compass:");
    Serial.println(compass);
  }
  String JSONtoString;
  // JSON object
  StaticJsonBuffer<200> jsonBuffer;
  JsonObject& objectJSON = jsonBuffer.createObject();
  objectJSON["idRobota"] = _idRobota;
  objectJSON["mov"] = (String)command;
  objectJSON["X"] = X;
  objectJSON["Y"] = Y;
  objectJSON["compass"] = (String)compass;
  // convert object JSON to string
  objectJSON.printTo(JSONtoString);
  webSocket.sendTXT(num,JSONtoString);
}
