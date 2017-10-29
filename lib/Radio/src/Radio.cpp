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

// Debug Radio
#define DEBUG_R 1

/**
 * init websocket server
 */
void Radio::init() {
  // stop motor reset output
  motors.stop();
  //init display
  multimedia.display_init();
  //turn off all leds
  multimedia.turnOFF();
  //define wifi parameters
  WifiConnection wificonnection;
  _idRobota = wificonnection.getSSID();
  // start webSocket server
  webSocket.begin();
  // init screen
  multimedia.display_update(PI_);
  // play welcome
  multimedia.buzzer_rttl(RTTL_WELCOME);

  // to define event use lambda and references
  // http://stackoverflow.com/questions/39803135/c-unresolved-overloaded-function-type
  // http://stackoverflow.com/questions/4940259/lambdas-require-capturing-this-to-call-static-member-function
  webSocket.onEvent([&](uint8_t num, WStype_t type, uint8_t * payload, size_t lenght){
    String stringWS;
    const char* commands;
    const char* ud;
    switch(type) {
        case WStype_DISCONNECTED:
            if(DEBUG_R) {
              Serial.printf("[%u] Disconnected!\n", num);
            }
            multimedia.display_update(PI_);
            break;
        case WStype_CONNECTED:
            // send message to client
            Radio::wssend(num, "Client connected");
            multimedia.display_update(SMILE);
            multimedia.movingLEDs(CRGB::Green);
            multimedia.movingLEDs(CRGB::DarkCyan);
            //multimedia.movingLEDs(CRGB::Green);
            //multimedia.movingLEDs(CRGB::DarkCyan);
            multimedia.buzzer_rttl(RTTL_FIDO);
            multimedia.led(LED_F,OFF);
            multimedia.led(LED_B,OFF);
            multimedia.led(LED_R,OFF);
            multimedia.led(LED_L,OFF);
            multimedia.led(LED_S,OFF);
            break;
        case WStype_TEXT:
            // Memory pool for JSON object tree.
            StaticJsonBuffer<200> rxjsonBuffer;
            if (DEBUG_R) {
              Serial.println();
              Serial.printf("[%u]: %s\n", num, payload);
            }
            // convert payload to JSON object
            // http://arduino.stackexchange.com/questions/30209/cast-from-uint8-t-to-char-loses-precision
            JsonObject& rxWS = rxjsonBuffer.parseObject((char *)payload);
            if (!rxWS.success()) {
              Serial.println("parseObject() failed");
              return;
            }
            // TODO comprobar que llega commands
            commands = rxWS["commands"];
            ud = rxWS["UD"];
            if (DEBUG_R) {
              Serial.println();
              Serial.printf("[commands]:%s:\n", commands);
              Serial.printf("[ud]:%s:\n", commands);
            }
            if (ud!=NULL) Radio::changeUD(num,ud);
            if (commands!=NULL) Radio::executCommands(num,commands);
            break;
    } // switch type of WS
  });
}

void Radio::changeUD(uint8_t num, const char* ud){
  multimedia.display_ud((String)ud);
}

void Radio::executCommands(uint8_t num, const char* commands){
  // ¿out of board?
  bool in;
  for (int i = 0; i < strlen((const char *)(commands)); i++) {
     if (DEBUG_R) {
       Serial.println((char)commands[i]);
     }
     // turn off the central led
     multimedia.led(LED_S, OFF);
     //Happy because execute command
     multimedia.display_update(SMILE);
     // action for different commands
     switch ((char)commands[i]) {
       case 'F':
        multimedia.led(LED_F, ON);
        in = motors.movForward(1);
        multimedia.buzzer_beep(TONE_FREQ_UP);
        multimedia.led(LED_F, OFF);
        break;
      case 'R':
        multimedia.led(LED_R, ON);
        motors.turnRight();
        multimedia.buzzer_beep(TONE_FREQ_RIGHT);
        multimedia.led(LED_R, OFF);
        break;
      case 'L':
        multimedia.led(LED_L, ON);
        motors.turnLeft();
        multimedia.buzzer_beep(TONE_FREQ_LEFT);
        multimedia.led(LED_L, OFF);
        break;
      case 'B':
        multimedia.led(LED_B, ON);
        in = motors.movBack(1);
        multimedia.buzzer_beep(TONE_FREQ_DOWN);
        multimedia.led(LED_B, OFF);
        break;
      // stop
      case 'S':
        motors.stop();
        multimedia.led(LED_S, ON);
        break;
    } // switch
    // display info
    multimedia.display_update(motors.getX(), motors.getY(), motors.getCardinal());
    // update display state
    if (!in) {
      multimedia.display_update(DISGUST);
      multimedia.buzzer_rttl(RTTL_MOSAIC);
    }
    // send coord
    wsexecuting(num,(char)commands[i],motors.getX(),motors.getY(),motors.getCardinal());
  } // loop evry commands
  // stop robota anyway after executing
  motors.stop();
  multimedia.display_update(WAIT);
}
/**
 * loop websocket server and update ui display
 */
 void Radio::wsloop() {
   webSocket.loop();
   multimedia.display_update();
 }
 /**
  *  send a broadcast, before create JSON
  */
void Radio::wsbroadcast(String msg){
  multimedia.display_heart(true);
  multimedia.display_update();
  String JSONtoString;
  // JSON object
  StaticJsonBuffer<200> jsonBuffer;
  JsonObject& objectJSON = jsonBuffer.createObject();
  objectJSON["idRobota"] = _idRobota;
  objectJSON["state"] = msg;
  // convert object JSON to string
  objectJSON.printTo(JSONtoString);
  webSocket.broadcastTXT(JSONtoString);
  multimedia.display_heart(false);
  multimedia.display_update();
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
  if (DEBUG_R) {
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
