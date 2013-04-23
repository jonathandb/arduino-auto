#include "auto.h"
#include "ir.h"


Motor motor;
IR ir;
//(&Input, &Output, &Setpoint,2,3,1, DIRECT);
//PID pidMotorSturing;

void setup()
{
  Serial.begin(9600);
}

void loop()
{
  ir.gatherIrValuesAndSort();
  calculateDirectionIrLight();
  int motorSpeed=ir.sensors[0].irValue/4;
  motor.adjust(200, 200);
  delay(100);
}

void calculateDirectionIrLight() {
  //ir light seen?
  if(ir.sensors[0].irValue > irTreshold) {
    if(ir.sensors[1].irValue < irTreshold) {
#ifdef DEBUG
      Serial.print(enumDirectionToString(ir.sensors[0].direction));
      Serial.print(" is the closest\n");
#endif

    if(ir.sensors[0].direction == frontLeft || ir.sensors[0].direction == backLeft) {
      addLatDistFromCenter(-1023);
    }
    else {
      addLatDistFromCenter(1023);
    }

    addDirectDistFromCenter(ir.sensors[0].irValue);
	}
    else {
#ifdef DEBUG
      Serial.print(enumDirectionToString(ir.sensors[0].direction));
      Serial.print(" and ");
      Serial.print(enumDirectionToString(ir.sensors[1].direction));
      Serial.print(" are the closest\n");
#endif

      //ir on front?
      if(ir.sensors[0].direction == frontLeft || ir.sensors[0].direction == frontRight && ir.sensors[1].direction == frontLeft || ir.sensors[1].direction == frontRight) {
        int latDistFromCenter;
        //12 o'clock is 0
        //CCW is negative
        //CW is positive
        if(ir.sensors[0].direction == frontLeft) {
          latDistFromCenter = ir.sensors[1].irValue - ir.sensors[0].irValue;
        } 
        else {
          latDistFromCenter = ir.sensors[0].irValue - ir.sensors[1].irValue;
        }        
        addLatDistFromCenter(latDistFromCenter);
        
        unsigned int directDistFromCenter = (abs(ir.sensors[0].irValue) + abs(ir.sensors[1].irValue)) / 2;
        addDirectDistFromCenter(directDistFromCenter);
      }
      //ir on back?
      else if(ir.sensors[0].direction == backLeft || ir.sensors[0].direction == backRight && ir.sensors[1].direction == backLeft || ir.sensors[1].direction == backRight) {
        int latDistFromCenter;
        if(ir.sensors[0].direction == backLeft) {
          latDistFromCenter = -1023;
        } 
        else {
          latDistFromCenter = 1023;
        }
        addLatDistFromCenter(latDistFromCenter);
        unsigned int directDistFromCenter = (abs(ir.sensors[0].irValue) + abs(ir.sensors[1].irValue)) / 2;
        addDirectDistFromCenter(directDistFromCenter);
      }
      //ir on left?
      else if(ir.sensors[0].direction == frontLeft || ir.sensors[0].direction == backLeft && ir.sensors[1].direction == frontLeft || ir.sensors[1].direction == backLeft) {
        addLatDistFromCenter(-1023);
        
        unsigned int directDistFromCenter = (abs(ir.sensors[0].irValue) + abs(ir.sensors[1].irValue)) / 2;
        addDirectDistFromCenter(directDistFromCenter);
      }
      //ir on right?
      else if(ir.sensors[0].direction == backRight || ir.sensors[0].direction == frontRight && ir.sensors[1].direction == backRight || ir.sensors[1].direction == frontRight) {
        addLatDistFromCenter(1023);
        
        unsigned int directDistFromCenter = (abs(ir.sensors[0].irValue) + abs(ir.sensors[1].irValue)) / 2;
        addDirectDistFromCenter(directDistFromCenter);
      }
    }
  } 
#ifdef DEBUG
    Serial.print("no ir found\n");
    int i;
    for (i = 0; i < 4; i++) {
      Serial.print("irValue: ");
      Serial.print(ir.sensors[i].irValue);
      Serial.print(" direction: ");
      Serial.print(enumDirectionToString(ir.sensors[i].direction));
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


