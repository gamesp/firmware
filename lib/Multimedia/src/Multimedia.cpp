/*
Copyright (C) 2017 Damian Nogueiras - Gamesp
This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.
This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.
You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301,
USA.
See LICENSE.txt for details
*/

#include "Multimedia.h"
#include <Wire.h>
// logo PI xbm format
#include "pi.h"
#include "smile.h"
#include "disgust.h"
#include "wait.h"
#include "ok.h"
#include "home.h"
#include "heart.h"
#include "heartbum.h"

// Debug multimedia
#define DEBUG_MU 1

// display
// Initialize the OLED display using Wire library
// D1 - D2 ; SLC - SDA
SSD1306  display(0x3c, D2, D1);
OLEDDisplayUi ui( &display );

Buzzer buzz = Buzzer(BUZZER_PIN);

InfoDisplay myInfo;

Multimedia::Multimedia(){
  //Initialize the rgb strip
  FastLED.addLeds<LED_TYPE, DATA_PIN, COLOR_ORDER>(_leds, NUM_LEDS).setCorrection( TypicalLEDStrip );
  FastLED.setBrightness(MAX_BRIGHTNESS);
  FastLED.clear();

  //Init info to display
  myInfo.x = 0;
  myInfo.y = 0;
  myInfo.ud = "X";
  myInfo.msg = "Up!";
  myInfo.board = "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA";
  //Init buzzer
  buzz.init();
}
/**
 * turn ON or OFF the index led
 * @param index of the array led
 * @param state ON or OFF
 */
void Multimedia::led(uint8_t ledNumber, uint8_t stateON) {
  if (stateON) {
    _leds[ledNumber] = _commandsColor[ledNumber];
    FastLED.show();
  } else {
    _leds[ledNumber] = CRGB::Black;
    FastLED.show();
  }
}
/**
 * turn OFF all leds
 */
void Multimedia::turnOFF() {
  fill_solid(_leds, 5, CRGB::Black);
  FastLED.show();
  // blue led off
  digitalWrite(D4, HIGH);
}
/**
 * move leds
 */
void Multimedia::movingLEDs(CRGB color) {
  for(int dot = 0; dot < NUM_LEDS; dot++) {
              _leds[dot] = color;
              FastLED.show();
              // clear this led for the next time around the loop
              _leds[dot] = CRGB::Black;
              delay(100);
          }
}

void msOverlay(OLEDDisplay *display, OLEDDisplayUiState* state) {
  display->setFont(ArialMT_Plain_10);
  String coord, ud;
  ud = "UD:";
  ud.concat(myInfo.ud);
  display->setTextAlignment(TEXT_ALIGN_LEFT);
  display->drawString(0, 0, ud);
  coord = "[";
  coord.concat(myInfo.x);
  coord.concat(",");
  coord.concat(myInfo.y);
  coord.concat(",");
  coord.concat(myInfo.compass);
  coord.concat("]");
  display->setTextAlignment(TEXT_ALIGN_RIGHT);
  display->drawString(128, 0, coord);
  display->drawXbm(110, 40, heart_width, heart_height, heart_bits);
  if (myInfo.heart) display->drawXbm(110, 40, heartbum_width, heartbum_height, heartbum_bits);
}
// frame 0 - PI
void drawFramePI(OLEDDisplay *display, OLEDDisplayUiState* state, int16_t x, int16_t y) {
  // draw an xbm image.
  display->drawXbm(32, 0, pi_width, pi_height, pi_bits);
}
// frame 1 - SMILE
void drawFrameSmile(OLEDDisplay *display, OLEDDisplayUiState* state, int16_t x, int16_t y) {
  // draw an xbm image.
  display->drawXbm(32, 0, smile_width, smile_height, smile_bits);
}
// frame 2 - DISGUST
void drawFrameDisgust(OLEDDisplay *display, OLEDDisplayUiState* state, int16_t x, int16_t y) {
  // draw an xbm image.
  display->drawXbm(32, 0, disgust_width, disgust_height, disgust_bits);
}
// frame 3 - WAIT
void drawFrameWait(OLEDDisplay *display, OLEDDisplayUiState* state, int16_t x, int16_t y) {
  // draw an xbm image.
  display->drawXbm(32, 0, wait_width, wait_height, wait_bits);
}
// frame 4 - Ok
void drawFrameOK(OLEDDisplay *display, OLEDDisplayUiState* state, int16_t x, int16_t y) {
  // draw an xbm image.
  display->drawXbm(32, 0, ok_width, ok_height, ok_bits);
}
// frame 5 - HOME
void drawFrameHome(OLEDDisplay *display, OLEDDisplayUiState* state, int16_t x, int16_t y) {
  // draw an xbm image.
  display->drawXbm(32, 0, home_width, home_height, home_bits);
}

// This array keeps function pointers to all frames
// frames are the single views that slide in
FrameCallback frames[]={drawFramePI, drawFrameSmile, drawFrameDisgust, drawFrameWait, drawFrameOK, drawFrameHome};

// how many frames are there?
int frameCount = 6;

// Overlays are statically drawn on top of a frame eg. a clock
OverlayCallback overlays[]={msOverlay};
int overlaysCount = 1;

void Multimedia::display_init() {
  // The ESP is capable of rendering 60fps in 80Mhz mode
	// but that won't give you much time for anything else
	// run it in 160Mhz mode or just set it to 30 fps
  ui.setTargetFPS(30);
  ui.setFrames(frames, frameCount);
  ui.setOverlays(overlays,overlaysCount);
  ui.disableAutoTransition();
  ui.disableAllIndicators();
  ui.init();
}
void Multimedia::display_update() {
  //update display
  if (ui.update() > 0) {
    //wait end draw
    delay(ui.update());
  }
}
void Multimedia::display_update(uint8_t state) {
  //update state
  myInfo.state = state;
  ui.switchToFrame(state);
  if (DEBUG_MU) {
    Serial.print("State(current frame):");
    Serial.println(ui.getUiState()->currentFrame);
  }
  display_update();
}
void Multimedia::display_update(int x, int y, char compass) {
  //update de coordinate
  myInfo.x = x;
  myInfo.y = y;
  myInfo.compass = compass;
  display_update();
}

void Multimedia::display_ud(String ud) {
  //update ud
  myInfo.ud = ud;
  display_update();
}

void Multimedia::display_heart(bool bum) {
  myInfo.heart = bum;
  if (bum) {
    //multimedia.led(LED_S, ON);
    digitalWrite(LED_BUILTIN,LOW);
  } else {
    // both blue leds off
    digitalWrite(LED_BUILTIN,HIGH);
    digitalWrite(D4, HIGH);
  }
  display_update();
}

void Multimedia::buzzer_beep(){
  buzz.beep();
}

void Multimedia::buzzer_beep(uint16_t frequency) {
  buzz.beep(frequency);
}

void Multimedia::buzzer_rttl(const char* rttl){
  buzz.playRttl(rttl);
}

String Multimedia::get_board(){
  return myInfo.board;
}

void Multimedia::set_board(String board){
  myInfo.board = board;
}
