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

const char* commands;
const char* ud;
const char* compass;
int coordX, coordY;

/**
 * init websocket server
 */
void Radio::init() {
  // stop motor reset output
  motors.stop();
  //init display
  multimedia.display_init();
  //define wifi parameters
  WifiConnection wificonnection;
  _idRobota = wificonnection.getSSID();
  // start webSocket server
  webSocket.begin();
  // init screen
  multimedia.display_update(HOME);
  // play welcome
  multimedia.buzzer_rttl(RTTL_WELCOME);
  //turn off all leds
  multimedia.turnOFF();

  // to define event use lambda and references
  // http://stackoverflow.com/questions/39803135/c-unresolved-overloaded-function-type
  // http://stackoverflow.com/questions/4940259/lambdas-require-capturing-this-to-call-static-member-function
  webSocket.onEvent([&](uint8_t num, WStype_t type, uint8_t * payload, size_t lenght){
    String stringWS;

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

            commands = rxWS["commands"];
            ud = rxWS["UD"];


            if (DEBUG_R) {
              Serial.println();
              Serial.printf("[commands]:%s:\n", commands);
              Serial.printf("[UD]:%s:\n", ud);
              Serial.print("[board]:");
              Serial.println(rxWS["board"].as<String>());
              Serial.print("[X]:");
              Serial.println(rxWS["X"].as<String>());
              Serial.print("[Y]:");
              Serial.println(rxWS["Y"].as<String>());
              Serial.printf("[compass]:%s:\n",compass);
            }
            // Change UD and load new board
            if (rxWS["X"]!=NULL && rxWS["Y"]!=NULL && rxWS["compass"]!=NULL ) {
                coordX = atoi(rxWS["X"]);
                coordY = atoi(rxWS["Y"]);
                compass = rxWS["compass"];
                Radio::changeXY(num,coordX,coordY,compass);
            }
            if (ud!=NULL) Radio::changeUD(num,ud,rxWS["board"].as<String>());
            // Execut commands
            if (commands!=NULL) Radio::executCommands(num,commands,rxWS["board"].as<String>());
            break;
    } // switch type of WS
  });
}

void Radio::changeXY(uint8_t num, int x, int y, const char* compass) {
    multimedia.display_update(x, y, compass[0]);
    motors.setCardinal(compass[0]);
    motors.setX(x);
    motors.setY(y);
}

void Radio::changeUD(uint8_t num, const char* ud, String board){
  String _ud = (String) ud;
  multimedia.set_board(board);
  multimedia.display_ud(_ud);
  // execute commands to update state
  const char stop[] = "S";
  executCommands(num, stop, board);
  //turn off all leds
  multimedia.turnOFF();
}

void Radio::executCommands(uint8_t num, const char* commands, String board){
  // ¿out of board?
  bool in;
  for (int i = 0; i < strlen((const char *)(commands)); i++) {
     if (DEBUG_R) {
       Serial.print("Executing ");
       Serial.println((char)commands[i]);
     }
     //turn off all leds
     multimedia.turnOFF();
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
    // Out of board
    if (!in) {
      multimedia.display_update(DISGUST);
      multimedia.buzzer_rttl(RTTL_MOSAIC);
    } else {
      // check board
      int _celda = MAXCELL*motors.getX()+motors.getY();
      String _board = multimedia.get_board();
      char est_celda = _board[MAXCELL*motors.getX()+motors.getY()];
      if (DEBUG_R) {
        Serial.print("Celda:");
        Serial.println((MAXCELL*motors.getX()+motors.getY()));
        Serial.print("Estado Celda:");
        Serial.println(est_celda);
      }
        switch (est_celda) {
          // P -> PI
          case 'P':
            multimedia.display_update(PI_);
            multimedia.buzzer_rttl(RTTL_INTEL);
            break;
          // A -> nothing
          case 'A':
            multimedia.display_update(SMILE);
            break;
          // X -> wrong
          case 'X':
            multimedia.display_update(DISGUST);
            multimedia.buzzer_rttl(RTTL_MOSAIC);
            break;
          // W -> wait
          case 'W':
            multimedia.display_update(WAIT);
            break;
          // O -> ok
          case 'O':
            multimedia.display_update(OK);
            multimedia.buzzer_rttl(RTTL_FIDO);
            break;
          // H -> Home
          case 'H':
            multimedia.display_update(HOME);
            break;
          case 'Z':
            multimedia.display_update(SLEEP);
            multimedia.sleep(CRGB::BlueViolet);
            break;
          default:
            multimedia.display_update(WAIT);
        }
    }
    // send coord
    wsexecuting(num,(char)commands[i],motors.getX(),motors.getY(),motors.getCardinal());
  } // loop evry commands
  // stop robota anyway after executing
  motors.stop();
  //multimedia.display_update(WAIT);
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
