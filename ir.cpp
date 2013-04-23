#include "ir.h"

#define irPinDelay 200


#define arraySize(x)  (sizeof(x) / sizeof(x[0]))

//adcpins
#define irPin1 0
#define irPin2 1
#define irPin3 2
#define irPin4 3

#define sensorsSize arraySize(IrSensor)


typedef int (IR::*compfn)(const void*, const void*);

IR::IR() {
  init();  
}

void IR::init() {
  //turn on pull up resistors too avoid floating input
  digitalWrite(irPin1, HIGH);
  digitalWrite(irPin2, HIGH);
  digitalWrite(irPin3, HIGH);
  digitalWrite(irPin4, HIGH);
}

void IR::gatherIrValuesAndSort() {
  // delay 10ms to let the ADC recover:
  delay(irPinDelay);
  unsigned int valueIr1 = analogRead(irPin1);  
  delay(irPinDelay);
  unsigned int valueIr2 = analogRead(irPin2);
  delay(irPinDelay);
  unsigned int valueIr3 = analogRead(irPin3);
  delay(irPinDelay);
  unsigned int valueIr4 = analogRead(irPin4);
  delay(irPinDelay);

  sensors[0] = (IrSensor) {
    valueIr1, frontLeft    };
  sensors[1] = (IrSensor) {
    valueIr2, frontRight    };
  sensors[2] = (IrSensor) {
    valueIr3, backLeft    };
  sensors[3] = (IrSensor) {
    valueIr4, backRight    };

  //http://www.cplusplus.com/reference/cstdlib/qsort/
  qsort((void *) &sensors, 4, sensorsSize, compareIrValues );              
}

int compareIrValues(const void* p1, const void* p2)
{
  struct IrSensor *irSensor1 = (struct IrSensor *)p1;
  struct IrSensor *irSensor2 = (struct IrSensor *)p2;

  if (irSensor1->irValue < irSensor2->irValue)
    return 1;

  else if (irSensor1->irValue > irSensor2->irValue)
    return -1;
  else
    return 0;
}
