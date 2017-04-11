#ifndef _MOTION_H
#define _MOTION_H

#include <stdint.h>
#include "Gear.h"

#define HOWMANYLOOPS 1 //how far does robota go, 1 to debug
#define MAXCELL 8 //number of cells

// Motion DEBUG 1 (true) 0 (false)
#define DEBUG 1

class Motion
{
public:

  /**
   * Constructor
   */
   Motion();
   /**
    * forward movement squares
    * @param squares Amount of squares on board. Negative go back
    */
    void movForward(int8_t squares);
    /**
     * back movement squares
     * @param squares Amount of squares on board
     */
    void movBack(int8_t squares){
      movForward(-squares);
    }
    /**
     * turn right-handed if param true or left-handed if param false
     * @param rightHanded if true turn right, if false turn left
     */
    void turn(bool rightHanded);
    /**
     * turn left handed
     */
    void turnLeft(){
      turn(false);
    }
    /**
     * turn right handed
     */
    void turnRight(){
      turn(true);
    }
    /**
     * stop robota
     */
    void stop();
    //movement X axis
    int steepX(int myCompass);
    //movement Y axis
    int steepY(int myCompass);
    // get private
    int getX();
    int getY();
    char getCardinal();
    // to dend i2c
    Gear gear;
private:
  //actual position on the board
  //init my position, right inf corner
  int _myPosition[2];
  // index of the cardinal point of my compass
  int _myCompass;
  //cardinal points
  const char _cardinal[4] = {'N','E','S','W'};
  //String _commands = "SFLBR";

};

#endif // _MOTION_H
