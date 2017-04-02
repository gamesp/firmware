
// DEBUG true (1) or false (0)
#define DEBUG 1
#define USE_SERIAL Serial

// id_connected
#define NONET     0  // no connection
#define MQTT      1  // there is wifi and mqtt broker connection
#define WEBLOCAL  2  // no wifi or no mqtt broker connection
uint8_t id_connected;
IPAddress apIP(192, 168, 4, 1);
IPAddress netMsk(255, 255, 255, 0);
// TODO concat a chip reference
String idRobota = "alfarobota";
