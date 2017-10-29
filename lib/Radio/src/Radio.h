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

class Radio {
  public:
    /**
     * Constructor
     */
     Radio(){};
     void init();
     void wsloop();
     void wsbroadcast(String msg);
     void wssend(uint8_t num, String msg);
     void wsexecuting(uint8_t num, char command, int X, int Y, char compass);
     void changeUD(uint8_t num, const char* ud);
     void executCommands(uint8_t num, const char* commands);
     // listen websocket at port 81
     WebSocketsServer webSocket = WebSocketsServer(81);
     // Motors of robota to mov
     Motion motors;
     // Sound and lights
     Multimedia multimedia;
   private:
     String _idRobota;
};

#endif // _RADIO_H
