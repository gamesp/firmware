#ifndef _MULTIMEDIA_H
#define _MULTIMEDIA_H

#include <FastLED.h>
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
// GPIO=3 it's D9
#define PIN_AUDIO 3


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
  void turnOFF();
  void movingLEDs(CRGB color);

private:
  // array of total leds
  CRGB _leds[NUM_LEDS];
  // different colour per commands
  CRGB _commandsColor[5] = {CRGB::Green,CRGB::Orange,CRGB::Green,CRGB::Orange,CRGB::Blue};
};

#endif // _MULTIMEDIA_H
