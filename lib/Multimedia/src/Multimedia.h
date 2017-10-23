#ifndef _MULTIMEDIA_H
#define _MULTIMEDIA_H

#include <FastLED.h>
// oled display library
#include <SSD1306.h>
// Include the UI lib
#include "OLEDDisplayUi.h"

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
#define PI 0
#define SMILE 1
#define DISGUST 2
#define WAIT 3

// info to display
typedef struct _InfoDisplay {
  int x=0;
  int y=0;
  char compass='N';
  String msg="";
  uint8_t state=0;
  bool heart = false;
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
  void display_init();
  void display_update();
  void display_update(uint8_t state);
  void display_update(int x, int y, char compass);
  void display_heart(bool bum);
  void msOverlay(OLEDDisplay *display, OLEDDisplayUiState* state);
  void drawFramePI(OLEDDisplay *display, OLEDDisplayUiState* state, int16_t x, int16_t y);
  void drawFrameSmile(OLEDDisplay *display, OLEDDisplayUiState* state, int16_t x, int16_t y);
  void drawFrameDisgust(OLEDDisplay *display, OLEDDisplayUiState* state, int16_t x, int16_t y);

private:
  // array of total leds
  CRGB _leds[NUM_LEDS];
  // different colour per commands
  CRGB _commandsColor[5] = {CRGB::Green,CRGB::Orange,CRGB::Green,CRGB::Orange,CRGB::Blue};
};

#endif // _MULTIMEDIA_H
