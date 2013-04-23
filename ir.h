#include "Arduino.h"

enum Direction {
  frontLeft, frontRight, backLeft, backRight };
const char * enumDirectionToString(Direction direction);

typedef struct IrSensor { 
  unsigned int  irValue;             
  Direction direction; 
};

int compareIrValues(const void* p1, const void* p2);

class IR
{
public:
  IR();
  void gatherIrValuesAndSort();
  struct IrSensor sensors[4];
private:
  //int compareIrValues(struct IrSensor *irSensor1, struct IrSensor *irSensor2);
  void init();
};
