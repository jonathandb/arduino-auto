#include "Arduino.h"

class Motor
{
public:
  Motor();
  void enable(boolean enable);
  void adjust(int speedMotor1, int speedMotor2);
  void testMotors();
private:
  void init();
  boolean _enable;
};
