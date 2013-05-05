/*
by Jonathan De Beir
 
 
 latDistFromCenterSize is an array that defines lateral distance of the IR light:
 IR on 12 o'clock: outOfDirections[0] = 0
 IR CCW : outOfDirections[0] is negative
 IR CW : outOfDirections[0] is positive
 
 */

#include "motor.h"

#define DEBUG
#define arraySize(x)  (sizeof(x) / sizeof(x[0]))



#define irTreshold 100
#define irIdeal 200
#define irIdealMargin 20
#define maxOutOfDirection 30
#define maxSerialLatency 100
#define cameraMessageLatency 100

#define onSwitchPin 11
#define infraredSwitchPin 12
#define cameraSwitchPin 13

void addDirectDistFromCenter(unsigned int latestDirectDistFromCenter);
int compareIrValues(struct IrSensor *irSensor1, struct IrSensor *irSensor2);

//list of current [0] and previous latDistFromCenter
int latDistFromCenter[10];
int directDistFromCenter[10];
#define latDistFromCenterSize arraySize(latDistFromCenter)
#define directDistFromCenterSize arraySize(directDistFromCenter)
unsigned int latDistFromCenterAverage;
unsigned int directDistFromCenterAverage;
unsigned int latDistFromCenterTotal;
unsigned int directDistFromCenterTotal;

typedef struct CameraPosition { 
  int directDist;
  int latDist;
};
unsigned long lastReceivedCameraMessageTime = 0;


struct CameraPosition cameraPosition;

enum Modus {
  off, 
  infrared, 
  camera, 
  hybrid
};

