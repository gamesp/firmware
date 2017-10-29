#ifndef _WIFICONNECTION_H
#define _WIFICONNECTION_H

#include <ESP8266WiFi.h>
#include <stdint.h>


class WifiConnection {
  public:
    /**
     * Constructor
     */
     WifiConnection();
     /**
      * COnfigure esp8266 only in mode Access Point, ip default
      * ssid Robota + chip id
      */
     void onlyAP();
     /**
      * return ssid, the same to id of robota
      */
      String getSSID();

 private:
   String _NameString = "Robota";
   const char * _AP_NameString;
   uint8_t _mac[6];
 };

#endif // _WIFICONNECTION_H
