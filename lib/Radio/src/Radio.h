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

// Radio DEBUG 1 (true) 0 (false)
#define DEBUG 1

// state of leds
#define ON 1  // true
#define OFF 0 // false
// number of the led at the strip
#define LED_F 2
#define LED_R 3
#define LED_B 0
#define LED_L 1
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
