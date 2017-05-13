#ifndef _MULTIMEDIA_H
#define _MULTIMEDIA_H

#include <FastLED.h>
#define FASTLED_ESP8266_NODEMCU_PIN_ORDER
// four commands and one to information
#define NUM_LEDS 5
// data pin of strip D4
#define DATA_PIN 4
// type of strip
#define LED_TYPE    WS2812
// this strip change de default order RGB to GRB
#define COLOR_ORDER GRB
// default brightness
#define MAX_BRIGHTNESS 16


class Multimedia
{
public:
  /**
   * Constructor
   */
  Multimedia();
  /**
   * turn ON or OFF the index led
   * @param state ON or OFF
   * @param index of the array led
   */
  void led(uint8_t index, uint8_t state);

private:
  // array of total leds
  CRGB _leds[NUM_LEDS];
  // different colour per commands
  CRGB _commandsColor[5] = {CRGB::Red,CRGB::Yellow,CRGB::Green,CRGB::Orange,CRGB::Blue};
};

#endif // _MULTIMEDIA_H
