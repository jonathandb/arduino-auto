#include "Arduino.h"
#include "motor.h"


#define DEBUG

// 255 is full speed clockwise
// -255 is full speed counterclockwise
//0 is brak

//pwm pins
#define motor1pin1 5
#define motor1pin2 6
#define motor2pin1 9
#define motor2pin2 10

//adc pins
#define testPotPin1 4
#define testPotPin2 5

//digital pins
#define testLedPin 13


//0 is no braking, 255 is max braking
#define brakeIntensity 50

#define motorSpeedChangeLatency 50

Motor::Motor() {
  init();
  _enable=true;
}

void Motor::init() {
  pinMode(motor1pin1, OUTPUT);
  pinMode(motor1pin1, OUTPUT);
  pinMode(motor2pin1, OUTPUT);
  pinMode(motor2pin2, OUTPUT);
  
  digitalWrite(testPotPin1, HIGH);
  digitalWrite(testPotPin2, HIGH);
  pinMode(testLedPin, OUTPUT);
  digitalWrite(testLedPin, LOW);
  
  enable(_enable);
}

void Motor::enable(boolean enable) {
  _enable = enable;  
  analogWrite(motor1pin1, 255);
  analogWrite(motor1pin2, 255);
  analogWrite(motor2pin1, 255);
  analogWrite(motor2pin2, 255);  
}

void Motor::changeSpeed(int speed1, int speed2) {
  desiredSpeed1 = speed1;
  desiredSpeed2 = speed2;
}

void Motor::updateSpeed() {
   if(millis() - lastChangedMotorTime > motorSpeedChangeLatency && _enable == true) {
    if(desiredSpeed1 > speedMotor1) {
      speedMotor1 += 10;
      if(desiredSpeed1 < speedMotor1) {
        speedMotor1 = desiredSpeed1;
      }
    } else if(desiredSpeed1 < speedMotor1) {
      speedMotor1 -= 10;
      if(desiredSpeed1 > speedMotor1) {
        speedMotor1 = desiredSpeed1;
      }      
    }
    if(desiredSpeed2 > speedMotor2) {
      speedMotor2 += 10;
      if(desiredSpeed2 < speedMotor2) {
        speedMotor2 = desiredSpeed2;
      }
    } else if(desiredSpeed2 < speedMotor2){
      speedMotor2 -= 10;
      if(desiredSpeed2 > speedMotor2) {
        speedMotor2 = desiredSpeed2;
      }      
    }
    lastChangedMotorTime = millis();
    adjust();
    #ifdef DEBUG
    Serial.print("Desired Speed motor 1,2 resp:");
    Serial.print("\t");
    Serial.print(desiredSpeed1);
    Serial.print("\t");
    Serial.print(desiredSpeed2);
    Serial.print("\n");
    Serial.print("Current Speed motor 1,2 resp:");
    Serial.print("\t");
    Serial.print(speedMotor1);
    Serial.print("\t");
    Serial.print(speedMotor2);
    Serial.print("\n\n");
    #endif
  }  
}

void Motor::adjust() {
  if(_enable == true) {
    if (speedMotor1 > 0) {
      analogWrite(motor1pin1, 255 - speedMotor1);
      analogWrite(motor1pin2, 255 - speedMotor1);
    } 
    else if (speedMotor1 < 0) {
      analogWrite(motor1pin1, 255);
      analogWrite(motor1pin2, 255 + speedMotor1);
    } 
    else if (speedMotor1 == 0) {
      analogWrite(motor1pin1, 255);
      analogWrite(motor1pin2, 255 - brakeIntensity);
    }

    if (speedMotor2 > 0) {
      analogWrite(motor2pin1, 255 - speedMotor2);
      analogWrite(motor2pin2, 255 - speedMotor2);
    } 
    else if (speedMotor2 < 0) {
      analogWrite(motor2pin1, 255);
      analogWrite(motor2pin2, 255 + speedMotor2);
    } 
    else if (speedMotor2 == 0) {
      analogWrite(motor2pin1, 255);
      analogWrite(motor2pin2, 255 - brakeIntensity);
    }
  }
}


void Motor::testMotors() {
  unsigned int valuePot1 = analogRead(testPotPin1);
  delay(10);
  unsigned int valuePot2 = analogRead(testPotPin2);
  delay(10);
  unsigned int speedMotor1 = 0;
  unsigned int speedMotor2 = 0;
  if (valuePot1>10) {
    speedMotor1 = valuePot1/4;
    digitalWrite(testLedPin, HIGH);
  } 
  else {
    digitalWrite(testLedPin, LOW);
  }

  if (valuePot2>10) {
    speedMotor2 = valuePot2/4;
  }
  adjust();
}
