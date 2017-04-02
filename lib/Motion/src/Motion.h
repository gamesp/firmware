#ifndef _MOTION_H
#define _MOTION_H

#include <stdint.h>

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
protected:

};

#endif // _MOTION_H
