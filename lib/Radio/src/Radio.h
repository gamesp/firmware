#ifndef _RADIO_H
#define _RADIO_H

#include <Arduino.h>
#include <stdint.h>
#include <Hash.h>
#include <WebSocketsServer.h>
#include <ArduinoJson.h>
#include "Motion.h"
#include "Multimedia.h"
#include "WifiConnection.h"
// FOTA update
#include <ESP8266HTTPClient.h>
#include <ESP8266httpUpdate.h>

// state of leds
#define ON 1  // true
#define OFF 0 // false
// number of the led at the strip
#define LED_F 0
#define LED_R 1
#define LED_B 2
#define LED_L 3
#define LED_S 4

class Radio {
  public:
    /**
     * Constructor
     */
     Radio(){};
     void init();
     void loop();
     void broadcast(String msg);
     void send(String msg, bool isMQTT);
     void executing(char command, int X, int Y, char compass);
     void rxparse(JsonObject& rx);
     void changeXY(int x, int y, const char* compass);
     void changeUD(const char* ud, int x, int y, const char* compass);
     void executCommands(char commands, char cell = 'A');
     void mqttConnection();
     void reconnect();
     void checkUpdates();
     void infoDisplay();
     bool setup_wifi();

     // listen websocket at port 81
     WebSocketsServer webSocket = WebSocketsServer(81);
     // Motors of robota to mov
     Motion motors;
     // Sound and lights
     Multimedia multimedia;
   private:
     String _idRobota;
     String _root = "/gamesp";
     bool _isMQTT = false;
     bool _isWifi = false;
     // Array of states
     char _cell_state[6] = {'A','X','O','W','H','Z'};
};

#endif // _RADIO_H
