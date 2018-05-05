#ifndef _WIFICONNECTION_H
#define _WIFICONNECTION_H

#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <stdint.h>

// Debug wifi connection
#define DEBUG_W 0

class WifiConnection {
  public:
    /**
     * Constructor
     */
     WifiConnection();
     /**
      * Configure esp8266 only in mode Access Point, ip default
      * ssid: "DOMOTTA + chip id"
      */
     void modeAP();
     bool wifiSetup(const char * ssid, const char * pass);
     bool wifiDebug();
     String connectionStatus (int which);
     /**
      * return ssid, the same to id of robota
      */
      String getSSID();
      /**
       * return AP, DOMOTTA-XXXX (mac)
       */
      const char * getAP();

 private:
   String _NameString = "DOMOTTA";
   const char * _AP_NameString;
   uint8_t _mac[6];
   const char *_ssid = "Test";
   const char *_pass = "Password";
 };

#endif // _WIFICONNECTION_H
