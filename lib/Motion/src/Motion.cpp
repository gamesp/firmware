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

#include "Motion.h"
#include <Arduino.h>

Motion::Motion(){
  _myPosition[0] = 9;
  _myPosition[1] = 9;
  _myCompass=0;
}

void Motion::movForward(int8_t squares){
  if (squares>0) {
    Serial.println("Go FORWARD");
    // X position
    _myPosition[0] = _myPosition[0] + steepX(_myCompass);
    // Y position
    _myPosition[1] = _myPosition[1] + steepY(_myCompass);
    gear.i2c('F',HOWMANYLOOPS);
  }
  else {
    Serial.println("Go Back");
    // X position
    _myPosition[0] = _myPosition[0] - steepX(_myCompass);
    // Y position
    _myPosition[1] = _myPosition[1] - steepY(_myCompass);
    gear.i2c('B',HOWMANYLOOPS);
  }
}

void Motion::turn(bool rightHanded){
  if (rightHanded) {
    Serial.println("Turn RIGHT");
    // change compass 90 degrees right
    if (_myCompass == 3) {
      //West to North
      _myCompass = 0;
    } else {
      _myCompass = _myCompass + 1;
    }
    gear.i2c('R',HOWMANYLOOPS);
  }
  else {
    Serial.println("Turn LEFT");
    // change compass 90 degrees left
    if (_myCompass == 0) {
      //West to North to West
      _myCompass = 3;
    } else {
      _myCompass = _myCompass - 1;
    }
    gear.i2c('L',HOWMANYLOOPS);
  }
}

void Motion::stop(){
  Serial.println("STOP");
  gear.i2c('S',1);
}
/**
 * movement X axis
 * @param myCompass cardinal direction
 */
int Motion::steepX(int myCompass) {
  switch (myCompass) {
    //North
    case 0:
      return -1;
    //East
    case 1:
      return 0;
    //South
    case 2:
      return +1;
    //West
    case 3:
      return 0;
    default:
      return 0;
  }
}

/**
 * movement Y axis
 * @param myCompass cardinal direction
 */
int Motion::steepY(int myCompass) {
  switch (myCompass) {
    //North
    case 0:
      return 0;
    //East
    case 1:
      return +1;
    //South
    case 2:
      return 0;
    //West
    case 3:
      return -1;
    default:
      return 0;
  }
}

/**
 * get X position
 */
int Motion::getX() {
  return _myPosition[0];
}
/**
 * get Y position
 */
int Motion::getY() {
  return _myPosition[1];
}
/**
 * get compass direction
 */
char Motion::getCardinal() {
  return _cardinal[_myCompass];
}
