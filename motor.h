#include "Arduino.h"


class Motor
{
public:
  Motor();
  void enable(boolean enable);
  void changeSpeed(int desiredSpeed1, int desiredSpeed2);
  void updateSpeed();
  void testMotors();
private:
  void init();
  void adjust();
  boolean _enable;
  int speedMotor1;
  int speedMotor2;
  int desiredSpeed1;
  int desiredSpeed2;
  unsigned long lastChangedMotorTime;
};
