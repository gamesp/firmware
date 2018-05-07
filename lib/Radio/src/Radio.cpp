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

String _topic = "";

// MQTT client
WiFiClient espClient;
PubSubClient client(espClient);

/**
 * init websocket server
 * @param isMQTT if true then init connection to broker MQTT
 */
void Radio::init() {
    //turn off all leds
    multimedia.turnOFF();
  // stop motor reset output
  motors.stop();
  //start AP
  wificonnection.modeAP();
  //define id with mac adress ssid = DOMOTTA-XXXX
  _idRobota = wificonnection.getSSID().substring(8);
  // init mqtt Wifi Connection
  if (_isMQTT) {
      _topic = _root + "/" + _idRobota;
      mqttConnection();
  }
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
            //multimedia.display_update(HOME);
            //multimedia.buzzer_rttl(RTTL_DISGUST);
            break;
        case WStype_CONNECTED:
            // send message to client
            Radio::send("connected");
            multimedia.display_update(SMILE);
            multimedia.buzzer_rttl(RTTL_FIDO);
            multimedia.movingLEDs();
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
            rxparse(rxWS);
            break;
    } // switch type of WS
  });
}
/**
 * parse de JSON obect and call executCommands to execute commands
 * @param rx the JSON object received
 */
void Radio::rxparse(JsonObject& rx){
    const char* commands;
    const char* ud;
    const char* compass;
    int coordX, coordY;
    const char* cells;
    unsigned int _length = 0;
    unsigned int _lengthcells = 0;
    const char* wifirequest;
    const char* update;

    commands = rx["commands"];
    ud = rx["UD"];
    compass = rx["compass"];
    coordX = rx["X"];
    coordY = rx["Y"];
    cells = rx["cells"];
    wifirequest = rx["wifi"];
    // require ota update
    update = rx["update"];

    // number of commands
    if (commands != NULL) _length = strlen(commands);
    // number of cells
    if (cells != NULL) _lengthcells = strlen(cells);

    if (DEBUG_R) {
      Serial.println("Parse......");
      Serial.print("[update]:");
      if (update!=NULL) Serial.println(update); else Serial.println("no update");
      Serial.print("[wifi]:");
      if (wifirequest!=NULL) Serial.println(wifirequest); else Serial.println("no setup wifi");
      Serial.print("[commands]:");
      if (commands!=NULL) Serial.println(commands); else Serial.println("no hay comandos");
      Serial.print("[cells]:");
      if (cells!=NULL) Serial.println(cells); else Serial.println("no hay estado celdas");
      Serial.printf("[UD]:");
      if (ud!=NULL) Serial.println(ud); else Serial.println("no hay cambio");
      Serial.print("[X]:");
      Serial.println(coordX);
      Serial.print("[Y]:");
      Serial.println(coordY);
      Serial.print("[compass]:");
      if (compass!=NULL) Serial.println(compass); else Serial.println("no hay compass");
      Serial.println();
    }
    // Change UD and XY
    if (compass!=NULL) Radio::changeXY(coordX,coordY,compass);
    if (ud!=NULL && compass!=NULL) Radio::changeUD(ud,coordX,coordY,compass);
    // setup WiFi
    if (wifirequest!=NULL) {
        //don´t show information, only icon
        multimedia.display_ud("Zleep");
        multimedia.display_update(WIFISETUP);
        multimedia.setWifi(_isWIFI);
        // reset attemps
        attemptsWifi = 3;
        attemptsMQTT = 3;
        _isWIFI=wificonnection.wifiSetup(rx["ssid"],rx["pass"]);
        if(!_isWIFI) {
            multimedia.display_update(DISGUST);
            multimedia.buzzer_rttl(RTTL_DISGUST);
            multimedia.led(LED_B, ON);
            delay(1000);
            multimedia.display_ud("?");
        }
        mqttConnection();
    }
    // Execut commands
    if (_length>0 && cells != NULL) {
      while (*commands) {
        if (DEBUG_R) {
          Serial.print("While "); Serial.print(commands[0]); Serial.println(cells[0]);
        }
        //TODO check _lengthcells == _length, cells[0] if not defined it containts trash
        executCommands(commands[0], cells[0]);
        // next command nest cell
        commands++;
        cells++;
      }
    }
    // check updates
    if (update != NULL && _isWIFI) checkUpdates();
}
void Radio::changeXY(int x, int y, const char* compass) {
    if (DEBUG_R) {
        Serial.println("Change XY,compass");
    }
    multimedia.display_update(x, y, compass[0]);
    motors.setCardinal(compass[0]);
    motors.setX(x);
    motors.setY(y);
    // execute commands to update state
    char stop = 'S';
    executCommands(stop, 'W');
}
void Radio::changeUD(const char* ud, int x, int y, const char* compass) {
  String _ud = (String) ud;
  multimedia.display_ud(_ud);
  if (DEBUG_R) {
      Serial.println("Change UD");
  }
  // change position
  changeXY(x, y, compass);
}
/**
 * Execute one command and send feedback to client
 * @param num id of websocket client
 * @param command to execute
 * @param cell state: SMILE, DISGUST, SURPRISE, WAIT, HOME, SLEEP
 */
void Radio::executCommands(char command, char cell){
  // ¿out of board?
  bool in;
  // send feedback
  //bool feedback = true;
     if (DEBUG_R) {
       Serial.print("Execut Command:");
       Serial.print((String)command);
       Serial.println(".");
     }
     //turn off all leds
     multimedia.turnOFF();

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
        //feedback = false;
        break;
    } // switch
    // display info
    multimedia.display_update(motors.getX(), motors.getY(), motors.getCardinal());
    // display state
    for (int i = 0; i < sizeof(_cell_state); i++) {
      if (_cell_state[i] == cell) {
          multimedia.display_update(i);
          if (DEBUG_R) {
            Serial.print("Update state:");
            Serial.print((String)cell);
            Serial.print("/");
            Serial.println(i);
          }
      }
    }
  // send coord to client ws and mqtt
  executing(command,motors.getX(),motors.getY(),motors.getCardinal());
  // stop robota anyway after executing
  motors.stop();
}
/**
 *  send a executing message to client
 */
void Radio::executing(char comando, int X, int Y, char compass){
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
  // send websocket broadcast
  webSocket.broadcastTXT(JSONtoString);
  if (_isMQTT) {
      // publish mqtt broker
      String topic_executing = _topic + "/executing";
      char buffer[objectJSONex.measureLength() + 1];
      objectJSONex.printTo(buffer, sizeof(buffer));
      client.publish(topic_executing.c_str(), buffer, true);
  }
}
/**
 * Mode MQTT
 */
void Radio::mqttConnection() {
    if (DEBUG_R) {
      Serial.println("MQTT setup connection");
    }
    // root topic
    _topic = _root + "/" + _idRobota;
    client.setServer(mqtt_server, mqtt_port);
    client.setCallback([&](char* topic, byte* payload, unsigned int length) {
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
    // Connect to broker
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
      // connected!!
      multimedia.display_update(SMILE);
      multimedia.buzzer_rttl(RTTL_FIDO);
      multimedia.movingLEDs();
      multimedia.display_ud("?");
    } else {
      //multimedia.display_update(DISGUST);
      //multimedia.display_ud("?");
      Serial.print("Failed MQTT connection, rc=");
      Serial.println(client.state());
      Serial.print(" try again in 5 scs, attemps:");
      Serial.println(attemptsMQTT);
      _isMQTT = false;
      attemptsMQTT--;
    }
}
/**
 * loop websocket server and update ui display
 */
void Radio::loop() {
   String msg;
   // websocket loop
   webSocket.loop();
   long now = millis();
   if (now - lastMsg > 5000) {
       // debug WiFi
       _isWIFI = wificonnection.wifiDebug();
       // mqtt loop
       if (_isWIFI) {
           //Serial.print("Mqtt state: ");
           //Serial.println(client.state());
            //if not connect init server
            //if (client.state()) mqttConnection();
            if (!client.connected()) {
               if (client.state()) mqttConnection();
               _isMQTT = false;
               reconnect();
               lastMsg = 0;
               /*if (attemptsMQTT=0) {
                   multimedia.buzzer_rttl(RTTL_DISGUST);
                   multimedia.led(LED_B, ON);
               }*/
            } else {
               client.loop();
            }
       } else {
           _isMQTT = false;
       }
       // broadcast message
       lastMsg = now;
       //keep alive
       ++value;
       msg = "{ON:";
       msg += value;
       //version
       msg +=",v:";
       msg += iFW_VERSION;
       // wifi state
       if (_isWIFI) msg+=",wifi:ok"; else msg+=",wifi:off";
       // mqtt state
       if (_isMQTT && _isWIFI) msg+=",mqtt:ok"; else msg+=",mqtt:off";
       // finish
       msg += "}";
       // msg to everybody
       broadcast(msg);
       multimedia.setWifi(_isMQTT);
   }
   // refresh display
   multimedia.display_update();
 }
/**
  *  send a broadcast, before must create JSON
  */
void Radio::broadcast(String msg){
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
  if (_isMQTT) {
      // publish mqtt
      String topic_state = _topic + "/state";
      client.publish(topic_state.c_str(), JSONtoString.c_str());
  }
  // turn off heart
  multimedia.display_heart(false);
  multimedia.display_update();
}
/**
 * send a state
 * @param num id of client
 * @param msg string with information
 */
void Radio::send(String msg){
  String JSONtoString;
  // JSON object
  StaticJsonBuffer<200> jsonBuffer;
  JsonObject& objectJSON = jsonBuffer.createObject();
  objectJSON["idRobota"] = _idRobota;
  objectJSON["state"] = msg;
  // convert object JSON to string
  objectJSON.printTo(JSONtoString);
  // send ws message
  webSocket.broadcastTXT(JSONtoString);
  // send mqtt message
  if (_isMQTT) {
      client.publish(_topic.c_str(), JSONtoString.c_str());
  }
}
/**
 * Check for update via OTA
 */
void Radio::checkUpdates() {
     // display update Mode
     //don´t show information, only icon
     multimedia.display_ud("Zleep");
     multimedia.display_update(UPDATE);
     int major = iFW_VERSION/100;
     int minor = (iFW_VERSION%100)/10;
     int revision = (iFW_VERSION%100)%10;
     String sFW_VERSION = String(major) + '.' + String(minor) + '.' + String(revision);
     const char* firmwareUrl = "http://gamesp.danielcastelao.org/versions/firmware.bin";
     const char* versionUrl = "http://gamesp.danielcastelao.org/versions/version.txt";
     HTTPClient httpClient;
     Serial.println( "Checking for firmware updates." );
     httpClient.begin( versionUrl );
     int httpCode = httpClient.GET();
     Serial.print("Http Code: ");
     Serial.println(httpCode);
     // if status OK
     if( httpCode == 200 ) {
          String snewFW_VERSION = httpClient.getString();
          int inewFW_VERSION = snewFW_VERSION.toInt();
          major = inewFW_VERSION/100;
          minor = (inewFW_VERSION%100)/10;
          revision = (inewFW_VERSION%100)%10;
          snewFW_VERSION = String(major) + '.' + String(minor) + '.' + String(revision);
          Serial.print( "Current firmware version: " );
          Serial.println( iFW_VERSION );
          Serial.print( "Available firmware version: " );
          Serial.println( inewFW_VERSION );

          if( inewFW_VERSION > iFW_VERSION ) {
              String _update = sFW_VERSION + String(">") + snewFW_VERSION;
              multimedia.display_ud(_update);
              Serial.println( "Preparing to update." );
              t_httpUpdate_return ret = ESPhttpUpdate.update( firmwareUrl );
              //t_httpUpdate_return ret = HTTP_UPDATE_OK;
              switch (ret) {
                  case HTTP_UPDATE_FAILED:
                    Serial.printf("HTTP_UPDATE_FAILD Error (%d): %s", ESPhttpUpdate.getLastError(), ESPhttpUpdate.getLastErrorString().c_str());
                    //check = false;
                    break;
                  case HTTP_UPDATE_NO_UPDATES:
                    Serial.println("HTTP_UPDATE_NO_UPDATES");
                    break;
                 case HTTP_UPDATE_OK:
                    Serial.println("HTTP_UPDATE_OK");
                    break;
            }
        } else {
            Serial.println( "Nothing to update." );
            multimedia.display_ud("?");
            multimedia.display_update(DISGUST);
            multimedia.buzzer_rttl(RTTL_DISGUST);
        }
      }
      httpClient.end();
 }
