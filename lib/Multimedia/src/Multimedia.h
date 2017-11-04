#ifndef _MULTIMEDIA_H
#define _MULTIMEDIA_H

#include <FastLED.h>
// oled display library
#include <SSD1306.h>
// Include the UI lib
#include "OLEDDisplayUi.h"
//Buzzer library
#include "Buzzer.h"

// to use chip name D4 and not GPIO2
#define FASTLED_ESP8266_NODEMCU_PIN_ORDER
// data pin of strip D4
#define DATA_PIN 4
// four commands and one to information
#define NUM_LEDS 5

// type of strip
#define LED_TYPE    WS2812
// this strip change de default order RGB to GRB
#define COLOR_ORDER GRB
// default brightness
#define MAX_BRIGHTNESS 16

// the frames to display
#define PI_ 0 // cell P
#define SMILE 1 // cell A
#define DISGUST 2 // out of board or cell X
#define WAIT 3 // wait a command
#define OK 4 //cell O
#define HOME 5 //cell H
#define SLEEP 6 //cell H


// info to display
typedef struct _InfoDisplay {
  int x=0;
  int y=0;
  char compass='N';
  String ud="X";
  String msg="";
  uint8_t state=0;
  bool heart = false;
  String board;
} InfoDisplay ;

class Multimedia
{
public:
  /**
   * Constructor
   */
  Multimedia();
  void led(uint8_t index, uint8_t state);
  void turnOFF();
  void movingLEDs(CRGB color);
  void sleep(CRGB color);
  void display_init();
  void display_update();
  void display_update(uint8_t state);
  void display_update(int x, int y, char compass);
  void display_ud(String ud);
  void display_heart(bool bum);
  void buzzer_beep();
  void buzzer_beep(uint16_t frequency);
  void buzzer_rttl(const char* rttl);
  String get_board();
  void set_board(String board);

private:
  // array of total leds
  CRGB _leds[NUM_LEDS];
  // different colour per commands
  CRGB _commandsColor[5] = {CRGB::Green,CRGB::Orange,CRGB::Green,CRGB::Orange,CRGB::Blue};
};

#endif // _MULTIMEDIA_H
