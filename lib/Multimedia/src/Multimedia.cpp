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

Multimedia::Multimedia(){
  //Initialize the rgb strip
  FastLED.addLeds<LED_TYPE, DATA_PIN, COLOR_ORDER>(_leds, NUM_LEDS).setCorrection( TypicalLEDStrip );
  FastLED.setBrightness(MAX_BRIGHTNESS);
  FastLED.clear();
  //Init pin buzzer
  pinMode(PIN_AUDIO, OUTPUT);

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
/**
 * Slides up or down from note1 to note2
 * @param nota1 first note
 * @param nota2 last note
 * @param tasa delay
 */
void Multimedia::glis(int nota1, int nota2, int tasa) {
  // By Dave Tucker
  //http://dtucker.co.uk/make/arduino-using-my-melodyutils-library-for-r2-d2-style-chirps.html

  if (nota1 < nota2) { //Slide up
    for (int nota = nota1; nota < nota2; nota++) {
      tone(PIN_AUDIO, nota, tasa); delay (tasa); noTone(PIN_AUDIO);
    }
  } else { //Slide down
    for (int nota = nota1; nota > nota2; nota--) {
      tone(PIN_AUDIO, nota, tasa); delay (tasa); noTone(PIN_AUDIO);
    }
  }
  noTone(PIN_AUDIO);
}
void Multimedia::sos() {
    tone(PIN_AUDIO, NOTE_A5, NOTA_CORCHEA); delay(NOTA_CORCHEA_PAUSA);
    tone(PIN_AUDIO, NOTE_A5, NOTA_CORCHEA); delay(NOTA_CORCHEA_PAUSA);
    tone(PIN_AUDIO, NOTE_A5, NOTA_CORCHEA); delay(NOTA_CORCHEA_PAUSA);
    tone(PIN_AUDIO, NOTE_G4, NOTA_NEGRA); delay(NOTA_NEGRA_PAUSA);
    tone(PIN_AUDIO, NOTE_F4, NOTA_NEGRA); delay(NOTA_NEGRA_PAUSA);
    tone(PIN_AUDIO, NOTE_C4, NOTA_NEGRA); delay(NOTA_NEGRA_PAUSA);
    noTone(PIN_AUDIO);
}
