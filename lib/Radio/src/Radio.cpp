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
// broker host and port
#include "brokerparam.h"

// Debug Radio
#define DEBUG_R 1

const char* commands;
const char* ud;
const char* compass;
int coordX, coordY;
String _topic = "";

// MQTT client
WiFiClient espClient;
PubSubClient client(espClient);

/**
 * init websocket server
 * @param isMQTT if true then init connection to broker MQTT
 */
void Radio::init(bool isMQTT) {
  _isMQTT = isMQTT;
  // stop motor reset output
  motors.stop();
  //define id with mac adress ssid = DOMOTTA-XXXX
  WifiConnection wificonnection;
  _idRobota = wificonnection.getSSID().substring(8);
  _topic = _root + "/" + _idRobota;
  //init display
  multimedia.display_init(_idRobota);
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
            Radio::send(num, "connected", false);
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
            // In order to republish or use this payload, a copy must be made
            // Allocate the correct amount of memory for the payload copy
            uint8_t* _payloadws = (uint8_t*)malloc(lenght);
            // Copy the payload to the new buffer
            memcpy(_payloadws,payload,lenght);
            // Memory pool for JSON object tree.
            StaticJsonBuffer<200> rxjsonBuffer;
            if (DEBUG_R) {
              Serial.println();
              Serial.printf("[%u]: %s\n", num, _payloadws);
            }
            // convert payload to JSON object
            // http://arduino.stackexchange.com/questions/30209/cast-from-uint8-t-to-char-loses-precision
            JsonObject& rxWS = rxjsonBuffer.parseObject((char *)_payloadws);
            if (!rxWS.success()) {
              Serial.println("parseObject() failed");
              return;
            }
            // parse the msg received
            rxparse(rxWS, num);
            break;
    } // switch type of WS
  });

  // init mqtt Wifi Connection
  if (_isMQTT) mqttConnection();
}
/**
 * parse de JSON obect and execute executCommands
 * @param rx the JSON object received
 * @param num identified the websocket connection
 */
void Radio::rxparse(JsonObject& rx, uint8_t num){
    unsigned int _length = 0;
    commands = rx["commands"];
    ud = rx["UD"];
    compass = rx["compass"];
    coordX = rx["X"];
    coordY = rx["Y"];
    // number of commands
    if (commands != NULL) _length = strlen(commands);
    if (DEBUG_R) {
      Serial.println("Parse......");
      Serial.printf("[commands]:");
      if (commands!=NULL) Serial.println(commands); else Serial.println("no hay comandos");
      Serial.printf("[UD]:");
      if (ud!=NULL) Serial.println(ud); else Serial.println("no hay cambio");
      /*Serial.print("[board]:");
      if (rx["board"]!=NULL) Serial.println(rx["board"].as<String>()); else Serial.println("no hay board");*/
      Serial.print("[X]:");
      Serial.println(coordX);
      Serial.print("[Y]:");
      Serial.println(coordY);
      Serial.print("[compass]:");
      if (compass!=NULL) Serial.println(compass); else Serial.println("no hay compass");
      Serial.println();
    }
    // Change UD and XY
    if (compass!=NULL) Radio::changeXY(num,coordX,coordY,compass);
    if (ud!=NULL) Radio::changeUD(num,ud,coordX,coordY,compass);
    // Execut commands
    if (_length>0) {
      while (*commands) {
        if (DEBUG_R) {
          Serial.print("dentro del while "); Serial.println(commands[0]);
        }
        executCommands(num, commands[0], "");
        // next command
        commands++;
      }
    }
}

void Radio::changeXY(uint8_t num, int x, int y, const char* compass) {
    if (DEBUG_R) {
        Serial.println("Change XY,compass");
    }
    multimedia.display_update(x, y, compass[0]);
    motors.setCardinal(compass[0]);
    motors.setX(x);
    motors.setY(y);
    // execute commands to update state
    char stop = 'S';
    executCommands(num, stop, "");
}

void Radio::changeUD(uint8_t num, const char* ud, int x, int y, const char* compass) {
  String _ud = (String) ud;
  multimedia.display_ud(_ud);
  if (DEBUG_R) {
      Serial.println("Change UD");
  }
  // change position
  changeXY(num, x, y, compass);
}

void Radio::executCommands(uint8_t num, char command, String board){
  // ¿out of board?
  bool in;
  // send feedback
  bool feedback = true;
     if (DEBUG_R) {
       Serial.print("Execut Command:");
       Serial.print((String)command);
       Serial.println(".");
     }
     //turn off all leds
     multimedia.turnOFF();
     //Happy because execute command
     // multimedia.display_update(SMILE);
     // action for different commands
     switch (command) {
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
      default :
        feedback = false;
        break;
    } // switch
    // display info
    multimedia.display_update(motors.getX(), motors.getY(), motors.getCardinal());
    /*
      // check cell
      int _celda = MAXCELL*motors.getX()+motors.getY();
      String _board = multimedia.get_board();
      char est_celda = _board[MAXCELL*motors.getX()+motors.getY()];
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
        }*/

  // send coord to client ws and mqtt
  if (feedback) executing(num,command,motors.getX(),motors.getY(),motors.getCardinal());
  // stop robota anyway after executing
  motors.stop();
  multimedia.display_update(WAIT);
}
/**
 *  send a executing message to client
 */
void Radio::executing(uint8_t num, char comando, int X, int Y, char compass){
  // to send object via websocket
  String JSONtoString;
  // JSON object
  StaticJsonBuffer<200> jsonBufferex;
  JsonObject& objectJSONex = jsonBufferex.createObject();
  objectJSONex["idRobota"] = _idRobota;
  objectJSONex["mov"] = (String)comando;
  objectJSONex["X"] = X;
  objectJSONex["Y"] = Y;
  objectJSONex["compass"] = (String)compass;
  // convert object JSON to string
  objectJSONex.printTo(JSONtoString);
  if (DEBUG_R) {
    Serial.print("Executing......");
    //objectJSONex.prettyPrintTo(Serial);
    Serial.println(JSONtoString);
  }
  // send over websocket
  if (num != NOWEBSOCK) webSocket.sendTXT(num,JSONtoString);
  // publish mqtt broker
  String topic_executing = _topic + "/executing";
  char buffer[objectJSONex.measureLength() + 1];
  objectJSONex.printTo(buffer, sizeof(buffer));
  client.publish(topic_executing.c_str(), buffer, true);
}

/**
 * Mode MQTT
 */
void Radio::mqttConnection() {
    if (DEBUG_W) {
      Serial.print("MQTT connection");
    }
    client.setServer(mqtt_server, mqtt_port);
    client.setCallback([&](char* topic, byte* payload, unsigned int length) {
      int num = NOWEBSOCK;
      // ¿out of board?
      bool in;
      bool feedback = true;
      int lengthMQTT = 0;
      // In order to republish or use this payload, a copy must be made
      // as the orignal payload buffer will be overwritten whilst
      // constructing the PUBLISH packet.
      // Allocate the correct amount of memory for the payload copy
      byte* _payload = (byte*)malloc(length);
      // Copy the payload to the new buffer
      memcpy(_payload,payload,length);

      if (DEBUG_R) {
        Serial.println();
        Serial.print("MQTT arrived [");
        Serial.print(topic);
        Serial.print("] ");
        for (int i = 0; i < length; i++) {
          Serial.print((char)_payload[i]);
        }
        Serial.println();
      }
        // Memory pool for JSON object tree.
        StaticJsonBuffer<200> rxjsonBuffer;
        // convert payload to JSON object
        // http://arduino.stackexchange.com/questions/30209/cast-from-uint8-t-to-char-loses-precision
        JsonObject& rxMQTT = rxjsonBuffer.parseObject((char *)_payload);
        if (!rxMQTT.success()) {
          Serial.println("parseObject() failed");
          return;
        }
        // parse the msg received
        rxparse(rxMQTT);
        // Free the memory
        free(_payload);
    });
    //delay(100);
}
/**
 * reconnect with MQTT broker
 */
void Radio::reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect(_idRobota.c_str())) {
      Serial.println("connected MQTT");
      // Once connected, publish an announcement...
      String _topicstate = _topic + "/state";
      String connectedMQTT = "{\"idRobota\":\""+_idRobota+"\",\"state\":\"connectedMQTT\"}";
      client.publish(_topicstate.c_str(), connectedMQTT.c_str());
      // ... and subscribe to subtopic commands
      String topic_commands = _topic + "/commands";
      client.subscribe(topic_commands.c_str());
      _isMQTT = true;
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
      _isMQTT = false;
    }
  }
}
/**
 * loop websocket server and update ui display
 */
void Radio::loop(bool isMQTT) {
   // websocket loop
   webSocket.loop();
   // mqtt loop
   //
   if (isMQTT) {
     if (!client.connected()) {
       reconnect();
     }
     client.loop();
   }
   // refresh display
   multimedia.display_update();
 }
/**
  *  send a broadcast, before must create JSON
  */
void Radio::broadcast(String msg, bool isMQTT){
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
  // send websocket broadcast
  webSocket.broadcastTXT(JSONtoString);
  // publish mqtt
  String topic_state = _topic + "/state";
  if (isMQTT) client.publish(topic_state.c_str(), JSONtoString.c_str());
  // turn off heart
  multimedia.display_heart(false);
  multimedia.display_update();
}
/**
 * send a state
 * @param num id of client
 * @param msg string with information
 */
void Radio::send(uint8_t num, String msg, bool isMQTT){
  String JSONtoString;
  // JSON object
  StaticJsonBuffer<200> jsonBuffer;
  JsonObject& objectJSON = jsonBuffer.createObject();
  objectJSON["idRobota"] = _idRobota;
  objectJSON["state"] = msg;
  // convert object JSON to string
  objectJSON.printTo(JSONtoString);
  // send ws message
  if (num != NOWEBSOCK) webSocket.sendTXT(num,JSONtoString);
  // send mqtt message
  if (isMQTT) client.publish(_topic.c_str(), JSONtoString.c_str());
}
