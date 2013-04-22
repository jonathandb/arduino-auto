

#include "auto.h"


enum Direction {
  frontLeft, frontRight, backLeft, backRight };
const char * enumDirectionToString(Direction direction);

struct IrSensor { 
  unsigned int  irValue;             
  Direction direction; 
};

typedef int (*compfn)(const void*, const void*);

struct IrSensor irSensors[4];
unsigned int sizeofIrSensor = sizeof(struct IrSensor);


Motor motor;
//(&Input, &Output, &Setpoint,2,3,1, DIRECT);
//PID pidMotorSturing;

void setup()
{
  Serial.begin(9600);
  initializePorts();
}

void loop()
{
  gatherIrValuesAndSort();
  calculateDirectionIrLight();
  int motorSpeed=irSensors[0].irValue/4;
  motor.adjust(200, 200);
  delay(100);
}

void gatherIrValuesAndSort() {
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

  irSensors[0] = (IrSensor) {
    valueIr1, frontLeft    };
  irSensors[1] = (IrSensor) {
    valueIr2, frontRight    };
  irSensors[2] = (IrSensor) {
    valueIr3, backLeft    };
  irSensors[3] = (IrSensor) {
    valueIr4, backRight    };

  //http://www.cplusplus.com/reference/cstdlib/qsort/
  qsort((void *) &irSensors, 4, sizeofIrSensor, (compfn)compareIrValues );              
}

void calculateDirectionIrLight() {
  //ir light seen?
  if(irSensors[0].irValue > irTreshold) {
    if(irSensors[1].irValue < irTreshold) {
#ifdef DEBUG
      Serial.print(enumDirectionToString(irSensors[0].direction));
      Serial.print(" is the closest\n");
#endif

    if(irSensors[0].direction == frontLeft || irSensors[0].direction == backLeft) {
      addLatDistFromCenter(-1023);
    }
    else {
      addLatDistFromCenter(1023);
    }

    addDirectDistFromCenter(irSensors[0].irValue);
	}
    else {
#ifdef DEBUG
      Serial.print(enumDirectionToString(irSensors[0].direction));
      Serial.print(" and ");
      Serial.print(enumDirectionToString(irSensors[1].direction));
      Serial.print(" are the closest\n");
#endif

      //ir on front?
      if(irSensors[0].direction == frontLeft || irSensors[0].direction == frontRight && irSensors[1].direction == frontLeft || irSensors[1].direction == frontRight) {
        int latDistFromCenter;
        //12 o'clock is 0
        //CCW is negative
        //CW is positive
        if(irSensors[0].direction == frontLeft) {
          latDistFromCenter = irSensors[1].irValue - irSensors[0].irValue;
        } 
        else {
          latDistFromCenter = irSensors[0].irValue - irSensors[1].irValue;
        }        
        addLatDistFromCenter(latDistFromCenter);
        
        unsigned int directDistFromCenter = (abs(irSensors[0].irValue) + abs(irSensors[1].irValue)) / 2;
        addDirectDistFromCenter(directDistFromCenter);
      }
      //ir on back?
      else if(irSensors[0].direction == backLeft || irSensors[0].direction == backRight && irSensors[1].direction == backLeft || irSensors[1].direction == backRight) {
        int latDistFromCenter;
        if(irSensors[0].direction == backLeft) {
          latDistFromCenter = -1023;
        } 
        else {
          latDistFromCenter = 1023;
        }
        addLatDistFromCenter(latDistFromCenter);
        
        unsigned int directDistFromCenter = (abs(irSensors[0].irValue) + abs(irSensors[1].irValue)) / 2;
        addDirectDistFromCenter(directDistFromCenter);
      }
      //ir on left?
      else if(irSensors[0].direction == frontLeft || irSensors[0].direction == backLeft && irSensors[1].direction == frontLeft || irSensors[1].direction == backLeft) {
        addLatDistFromCenter(-1023);
        
        unsigned int directDistFromCenter = (abs(irSensors[0].irValue) + abs(irSensors[1].irValue)) / 2;
        addDirectDistFromCenter(directDistFromCenter);
      }
      //ir on right?
      else if(irSensors[0].direction == backRight || irSensors[0].direction == frontRight && irSensors[1].direction == backRight || irSensors[1].direction == frontRight) {
        addLatDistFromCenter(1023);
        
        unsigned int directDistFromCenter = (abs(irSensors[0].irValue) + abs(irSensors[1].irValue)) / 2;
        addDirectDistFromCenter(directDistFromCenter);
      }
    }
  } 
#ifdef DEBUG
    Serial.print("no ir found\n");
    int i;
    for (i = 0; i < 4; i++) {
      Serial.print("irValue: ");
      Serial.print(irSensors[i].irValue);
      Serial.print(" direction: ");
      Serial.print(enumDirectionToString(irSensors[i].direction));
      Serial.print("\t\n");
    }
    Serial.print("latDistFromCenter: ");
    Serial.print(latDistFromCenter[0]);
    Serial.print("latDistFromCenterAverage: ");
    Serial.print(latDistFromCenterAverage);
    Serial.print("\n");
    Serial.print("directDistFromCenterAverage: ");
    Serial.print(directDistFromCenterAverage);
    Serial.print("\n\n");
#endif
}

void initializePorts() {
  //turn on pull up resistors too avoid floating input
  digitalWrite(irPin1, HIGH);
  digitalWrite(irPin2, HIGH);
  digitalWrite(irPin3, HIGH);
  digitalWrite(irPin4, HIGH);
}

const char * enumDirectionToString(Direction direction) {
  switch(direction)
  {
  case frontLeft:
    return "frontLeft";
  case frontRight:
    return "frontRight";
  case backLeft:
    return "backLeft";
  case backRight:
    return "backRight";
  }
  return "direction not found";
}

int compareIrValues(struct IrSensor *irSensor1, struct IrSensor *irSensor2)
{
  if ( irSensor1->irValue < irSensor2->irValue)
    return 1;

  else if (irSensor1->irValue > irSensor2->irValue)
    return -1;

  else
    return 0;
}

void addLatDistFromCenter(int latestlatDistFromCenter) {
  latDistFromCenterTotal -= latDistFromCenter[latDistFromCenterSize - 1];
  
  for(int i = 0; i < latDistFromCenterSize - 1; i++) {
    latDistFromCenter[i + 1] = latDistFromCenter[i];
  } 
  latDistFromCenter[0] = latestlatDistFromCenter;
  
  latDistFromCenterTotal += latDistFromCenter[0];
  latDistFromCenterAverage = latDistFromCenterTotal / latDistFromCenterSize;
  
}

void addDirectDistFromCenter(unsigned int latestDirectDistFromCenter) {
  directDistFromCenterTotal -= directDistFromCenter[directDistFromCenterSize - 1];
  
  for(int i = 0; i < directDistFromCenterSize - 1; i++) {
    directDistFromCenter[i + 1] = directDistFromCenter[i];
  } 
  directDistFromCenter[0] = latestDirectDistFromCenter;
  
  directDistFromCenterTotal += directDistFromCenter[0];
  directDistFromCenterAverage = latDistFromCenterTotal / latDistFromCenterSize;
}


