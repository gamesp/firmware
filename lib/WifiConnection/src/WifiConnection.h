#ifndef _WIFICONNECTION_H
#define _WIFICONNECTION_H

#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <stdint.h>


class WifiConnection {
  public:
    /**
     * Constructor
     */
     WifiConnection();
     /**
      * Configure esp8266 only in mode Access Point, ip default
      * ssid: "Robota + chip id"
      */
     void onlyAP();
     /**
      * Configure wifi-internet and MQTT
      */
     void MQTT();
     /**
      * return ssid, the same to id of robota
      */
      String getSSID();
      /**
       * Callback function for events

      void callback();*/

 private:
   String _NameString = "DOMOTTA";
   const char * _AP_NameString;
   uint8_t _mac[6];
   const char* mqtt_server = "broker.mqtt-dashboard.com";
 };

#endif // _WIFICONNECTION_H
