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

}

void Motion::movForward(int8_t squares){
  if (squares>0)
    Serial.println("Go FORWARD");
  else
    Serial.println("Go Back");
}

void Motion::turn(bool rightHanded){
  if (rightHanded)
    Serial.println("Turn RIGHT");
  else
    Serial.println("Turn LEFT");
}

void Motion::stop(){
  Serial.println("STOP");
}
