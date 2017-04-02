#ifndef _RADIO_H
#define _RADIO_H

#include <stdint.h>
#include <WebSocketsServer.h>
#include <Arduino.h>
#include "Motion.h"
#include <ArduinoJson.h>

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
};

#endif // _RADIO_H
