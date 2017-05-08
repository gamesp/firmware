#ifndef _GEAR_H
#define _GEAR_H

#define DELAYSTEP 5 //delay between step
#define ADDRESS_PFC 32 //32 placa , 63 protoboard

// Motion DEBUG 1 (true) 0 (false)
#define DEBUG 1

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
   * @loop how many times repeat de pattern
   */
  void i2c(char direction, int loop);

private:
  int _error;

};

#endif // _GEAR_H
