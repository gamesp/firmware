/**
 * General Config
 */
#ifndef _CONFIG_H
#define _CONFIG_H
// Debug in loop true (1) or false (0)
#define DEBUG 1

// id_connected
#define NONET     0  // no connection
#define WIFIMQTT      1  // there is wifi and mqtt broker connection
#define WEBLOCAL  2  // no wifi or no mqtt broker connection
uint8_t id_connected;

#define STATE       0  // type msg to broadcast
#define EXECUTING   1  // msg type to send what executing

#endif // _CONFIG_H
