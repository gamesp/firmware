#ifndef _RADIO_H
#define _RADIO_H

#include <stdint.h>
#include <WebSocketsServer.h>

class Radio
{
public:
  /**
   * Constructor
   */
   Radio(){};
   void init();
   void wsloop();
   void wsbroadcast(String msg);
   void wssend(uint8_t num, String msg);
   // listen websocket at port 81
   WebSocketsServer webSocket = WebSocketsServer(81);

protected:

};

 #endif // _RADIO_H
