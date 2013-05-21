#include "Arduino.h"
#include "motor.h"


#define DEBUG

//pwm pins
#define pinMotor1 9
#define pinMotor2 10


//adc pins
#define testPotPin1 11
#define testPotPin2 12

//digital pins
#define testLedPin 13


//0 is no braking, 255 is max braking
#define brakeIntensity 50

#define motorSpeedChangeLatency 50

Motor::Motor() {
  init();
}

void Motor::init() {
  pinMode(pinMotor1, OUTPUT);
  pinMode(pinMotor2, OUTPUT);
  //set pwm frequenct to 10Hz (31250/1024=30.5)
  setPwmFrequency(testPotPin1, 1024);
  setPwmFrequency(testPotPin2, 1024);
  
  digitalWrite(testPotPin1, HIGH);
  digitalWrite(testPotPin2, HIGH);
  pinMode(testLedPin, OUTPUT);
  digitalWrite(testLedPin, LOW);
  
}

void Motor::changeSpeed(int speed1, int speed2) {
  desiredSpeed1 = speed1;
  desiredSpeed2 = speed2;
  #ifdef DEBUG
  Serial.print("Motor1, motor2: ");
  Serial.print(desiredSpeed1);
  Serial.print("\t");
  Serial.print(desiredSpeed2);
  Serial.print("\n\n");
  #endif
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
    #ifdef MINDEBUG
    Serial.print(desiredSpeed1);
    Serial.print("\t");
    Serial.print(desiredSpeed2);
    Serial.print("\t\t");
    Serial.print(speedMotor1);
    Serial.print("\t");
    Serial.print(speedMotor2);
    Serial.print("\n");
    #endif
  }  
}

void Motor::adjust() {
  analogWrite(pinMotor1, speedMotor1);
  analogWrite(pinMotor2, speedMotor2);
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

/**
 * Divides a given PWM pin frequency by a divisor.
 * 
 * The resulting frequency is equal to the base frequency divided by
 * the given divisor:
 *   - Base frequencies:
 *      o The base frequency for pins 3, 9, 10, and 11 is 31250 Hz.
 *      o The base frequency for pins 5 and 6 is 62500 Hz.
 *   - Divisors:
 *      o The divisors available on pins 5, 6, 9 and 10 are: 1, 8, 64,
 *        256, and 1024.
 *      o The divisors available on pins 3 and 11 are: 1, 8, 32, 64,
 *        128, 256, and 1024.
 * 
 * PWM frequencies are tied together in pairs of pins. If one in a
 * pair is changed, the other is also changed to match:
 *   - Pins 5 and 6 are paired on timer0
 *   - Pins 9 and 10 are paired on timer1
 *   - Pins 3 and 11 are paired on timer2
 * 
 * Note that this function will have side effects on anything else
 * that uses timers:
 *   - Changes on pins 3, 5, 6, or 11 may cause the delay() and
 *     millis() functions to stop working. Other timing-related
 *     functions may also be affected.
 *   - Changes on pins 9 or 10 will cause the Servo library to function
 *     incorrectly.
 * 
 * Thanks to macegr of the Arduino forums for his documentation of the
 * PWM frequency divisors. His post can be viewed at:
 *   http://www.arduino.cc/cgi-bin/yabb2/YaBB.pl?num=1235060559/0#4
 */
void Motor::setPwmFrequency(int pin, int divisor) {
  byte mode;
  if(pin == 5 || pin == 6 || pin == 9 || pin == 10) {
    switch(divisor) {
      case 1: mode = 0x01; break;
      case 8: mode = 0x02; break;
      case 64: mode = 0x03; break;
      case 256: mode = 0x04; break;
      case 1024: mode = 0x05; break;
      default: return;
    }
    if(pin == 5 || pin == 6) {
      TCCR0B = TCCR0B & 0b11111000 | mode;
    } else {
      TCCR1B = TCCR1B & 0b11111000 | mode;
    }
  } else if(pin == 3 || pin == 11) {
    switch(divisor) {
      case 1: mode = 0x01; break;
      case 8: mode = 0x02; break;
      case 32: mode = 0x03; break;
      case 64: mode = 0x04; break;
      case 128: mode = 0x05; break;
      case 256: mode = 0x06; break;
      case 1024: mode = 0x7; break;
      default: return;
    }
    TCCR2B = TCCR2B & 0b11111000 | mode;
  }
}
