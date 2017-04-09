#ifndef _GEAR_H
#define _GEAR_H

#define DELAYSTEP 200 //delay between step

#include <Arduino.h>
#include <Wire.h>
#include <pcf8574_esp.h>

class Gear
{
public:
  // Constructor
  Gear();
  /**
   * Send byte to pf7584 with pattern to mov bobine
   * @param direction compass position
   * @loop how many times repeat de pattern*/
  void i2c(char direction, int loop);

private:

};

#endif // _GEAR_H
