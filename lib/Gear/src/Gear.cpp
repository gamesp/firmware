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

#include "Gear.h"
// Debug Gear
#define DEBUG_G 0

/*  Wire.h already defines "Wire" which the PCF8574-class would use by default,
    but we define our own instance of it and use that instead!

    Also, since I2C is emulated on the ESP8266 let's redefine what pins to use as SDA and SCL
    and instead swap them around!
    SDA=GPIO4=D2, SCL=GPIO5=D1 (ON NODEMCU) */
TwoWire testWire;

// Initialize a PCF8574AT at I2C-address 0x3F(63d) // pcf7857 placa 0x20(32d), using GPIO5, GPIO4 and testWire for the I2C-bus
// TODO include scanner to detect address
PCF857x pcf8574(ADDRESS_PFC, &testWire);
/**
 *  Half step, soft movement

const static byte step_patternFB[] = {
    B00010001, B00110011, B00100010, B01100110, B01000100, B11001100, B10001000, B10011001
};
const static byte step_patternLR[] = {
    B00011001, B00111000, B00101100, B01100100, B01000110, B11000010, B10000011, B10010001
};
*/
/**
 *  wave drive, low consum ¿more fast?
 */

 const static byte step_patternFB[] = {
     B00011000, B00100100, B01000010, B10000001
 };
 const static byte step_patternLR[] = {
     B10001000, B01000100, B00100010, B00010001
 };

Gear::Gear(){
  testWire.begin(4, 5);
  //Specsheets say PCF8574 is officially rated only for 100KHz I2C-bus
  //PCF8575 is rated for 400KHz
  testWire.setClock(100000L);

  pcf8574.begin();
  _error = pcf8574.lastError();
}

// send to bus i2c de pattern
void Gear::i2c(char direction, int loop){
  if (DEBUG_G) {
    Serial.print("Error before mov:");
    Serial.println(_error);
  }

  switch (direction) {
    case 'F':
    for (int i=0; i<loop; i++) {
      int index=3;
      do {
        if (DEBUG_G) {
          Serial.println(step_patternFB[index],BIN);
        }
        pcf8574.write8(step_patternFB[index]);
        delay(DELAYSTEP);
        index--;
      } while (index>-1);
    };
      break;
    case 'B':
      for (int i=0; i<loop; i++) {
        for (int index=0; index<4; index++){
          if (DEBUG_G) {
            Serial.println(step_patternFB[index],BIN);
          }
          pcf8574.write8(step_patternFB[index]);
          delay(DELAYSTEP);
        }
      };
      break;
    case 'L':
      for (int i=0; i<loop; i++) {
        for (int index=0; index<4; index++){
          if (DEBUG_G) {
            Serial.println(step_patternLR[index],BIN);
          }
          pcf8574.write8(step_patternLR[index]);
          delay(DELAYSTEP);
        }
      };
    break;
    case 'R':
      for (int i=0; i<loop; i++) {
        int index=3;
        do {
          if (DEBUG_G) {
            Serial.println(step_patternLR[index],BIN);
          }
          pcf8574.write8(step_patternLR[index]);
          delay(DELAYSTEP);
          index--;
        } while (index>-1);
      };
    break;
    default :
      // stop
      if (DEBUG_G) {
        Serial.println(B00000000,BIN);
      }
      pcf8574.write8(0x00);
  }
  // take error of last movement
  _error = pcf8574.lastError();
}
