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

// state of leds
#define ON 1  // true
#define OFF 0 // false
// number of the led at the strip
#define LED_F 0
#define LED_R 1
#define LED_B 2
#define LED_L 3
#define LED_S 4

#define NOWEBSOCK 9

class Radio {
  public:
    /**
     * Constructor
     */
     Radio(){};
     void init(bool isMQTT);
     void loop(bool isMQTT);
     void broadcast(String msg, bool isMQTT);
     void send(uint8_t num, String msg, bool isMQTT);
     void executing(uint8_t num, char command, int X, int Y, char compass);
     void rxparse(JsonObject& rx, uint8_t num = 9);
     void changeXY(uint8_t num, int x, int y, const char* compass);
     void changeUD(uint8_t num, const char* ud, int x, int y, const char* compass);
     void executCommands(uint8_t num, char commands, char cell = 'A');
     void mqttConnection();
     void reconnect();

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
     // Array of states
     char _cell_state[6] = {'A','X','O','W','H','Z'};
};

#endif // _RADIO_H
