#include "auto.h"
#include "ir.h"


Motor motor;
IR ir;
Modus modus = off;
Modus previousModus;
//(&Input, &Output, &Setpoint,2,3,1, DIRECT);
//PID pidMotorSturing;

void setup()
{
  Serial.begin(9600);
  pinMode(onSwitchPin, INPUT);
  pinMode(infraredSwitchPin, INPUT);
  pinMode(cameraSwitchPin, INPUT);
}

void loop()
{
  if (checkModus()) {
    switch(modus)
    {
    case off:
      motor.adjust(0, 0);
      return;
    case camera:
      lastReceivedCameraMessageTime = millis();
      //if correct message is received, control the motor
      if(receiveSerialData()) {
        controlMotorWithCamera();
      } else {
        //if the last message takes longer than maxSerialLatency, then the motor is stopped
        if (millis() - cameraMessageLatency > maxSerialLatency) {
          motor.adjust(0, 0);
        }
      }
    case infrared:
      ir.gatherIrValuesAndSort();
      calculateDirectionIrLight();
      controlMotorWithIr();
    case hybrid:
      ir.gatherIrValuesAndSort();
      calculateDirectionIrLight();      
      //if correct message is received, control the motor
      if(receiveSerialData()) {
        controlMotorWithIrAndCamera();
      } else {
        //if the last message takes longer than maxSerialLatency, then only the infrared sensors are used to control the motor
        if (millis() - cameraMessageLatency > maxSerialLatency) {
          motor.adjust(0, 0);
        }
      }
      receiveSerialData();
    }
  }
  
  delay(100);
}

void controlMotorWithCamera() {
  //if distance is 700 speed is ideal, directDistFromCenteAverage > 700 => less speed
  int turnSpeed = (cameraPosition.latDist / 4);
  int distanceSpeed = (100 / cameraPosition.directDist);
  int motor1Speed = distanceSpeed + turnSpeed;
  int motor2Speed = distanceSpeed - turnSpeed;
  
  if (motor1Speed > 255) {
    motor1Speed = 255; 
  }
  if (motor1Speed < -255) {
    motor1Speed = -255; 
  }
  if (motor2Speed > 255) {
    motor2Speed = 255; 
  }
  if (motor2Speed < -255) {
    motor2Speed = -255; 
  }
  motor.adjust(motor1Speed, motor2Speed);
}

void controlMotorWithIr() {
  //if distance is 700 speed is ideal, directDistFromCenteAverage > 700 => less speed
  int turnSpeed = (latDistFromCenterAverage / 4);
  int distanceSpeed = abs((100 / directDistFromCenterAverage) - abs(turnSpeed));
  int motor1Speed = distanceSpeed + turnSpeed;
  int motor2Speed = distanceSpeed - turnSpeed;
  
  if (motor1Speed > 255) {
    motor1Speed = 255; 
  }
  if (motor1Speed < -255) {
    motor1Speed = -255; 
  }
  if (motor2Speed > 255) {
    motor2Speed = 255; 
  }
  if (motor2Speed < -255) {
    motor2Speed = -255; 
  }
  motor.adjust(motor1Speed, motor2Speed);
}

void controlMotorWithIrAndCamera() {
  //if distance is 700 speed is ideal, directDistFromCenteAverage > 700 => less speed
  int turnSpeed1 = (cameraPosition.latDist / 4);
  int turnSpeed2 = (latDistFromCenterAverage / 4);
  int turnSpeed = (turnSpeed1 + turnSpeed2) / 2;
  int distanceSpeed = (100 / cameraPosition.directDist);
  int motor1Speed = distanceSpeed + turnSpeed;
  int motor2Speed = distanceSpeed - turnSpeed;
  
  if (motor1Speed > 255) {
    motor1Speed = 255; 
  }
  if (motor1Speed < -255) {
    motor1Speed = -255; 
  }
  if (motor2Speed > 255) {
    motor2Speed = 255; 
  }
  if (motor2Speed < -255) {
    motor2Speed = -255; 
  }
  motor.adjust(motor1Speed, motor2Speed);
}



boolean checkModus() {
  if(!digitalRead(onSwitchPin)) {
    modus = off;
  } else if(digitalRead(infraredSwitchPin)) {
    modus = infrared;
  } else if(digitalRead(cameraSwitchPin)) {
    modus = camera;
  } else {
    modus = hybrid;
  }
  
  if(modus == previousModus) {
    previousModus = modus;
    return false;
  } else {
    previousModus = modus;
    return true;
  }
}

boolean receiveSerialData() {
  if (Serial.available() > 0) {
    byte initByte = Serial.read();
#ifdef DEBUG
    Serial.print("First received byte is: ");
    Serial.print(initByte);
    Serial.print("\n");
#endif
    if(initByte == 0xFF) {
      unsigned long startTime = millis();
      while ((Serial.available()<4) && ((millis()-startTime) < maxSerialLatency))
      {      
        // wait until we get 4 bytes of data or maxSerialLatency has gone by
      }
      byte firstByte = Serial.read();
      byte secondByte = Serial.read();
      byte thirdByte = Serial.read();
      byte fourthByte = Serial.read();
#ifdef DEBUG
      Serial.print("byte 1 is: ");
      Serial.print(firstByte);
      Serial.print("\n");
      Serial.print("byte 2 is: ");
      Serial.print(secondByte);
      Serial.print("\n");
      Serial.print("byte 3 is: ");
      Serial.print(thirdByte);
      Serial.print("\n");
      Serial.print("byte 4 is: ");
      Serial.print(fourthByte);
      Serial.print("\n");
#endif
      lastReceivedCameraMessageTime = millis();
      int directDist = firstByte + (secondByte << 8);
      int latDist = thirdByte + (fourthByte << 8);
      cameraPosition = (CameraPosition) {
        directDist,latDist};
      return true;
    }
  }
  return false;
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







