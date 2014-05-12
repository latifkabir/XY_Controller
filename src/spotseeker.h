#ifndef RX130_H
#define RX130_H

#include "propeller.h"

struct Vector4
{
 double x;
 double y;
 double z;
 double r;
};

class SpotSeeker
{
public:
 void performDemo();
 int moveX(int cycles);
 int moveY(int cycles);
 int moveZ(int cycles);
 int moveR(int cycles);
 Vector4 moveTo(double x, double y, double z);

 Vector4 getPosition();

 void setOrigin();
 SpotSeeker();
 ~SpotSeeker();
 int moveLink(int DOF, int clicks, const bool steady);

private:
  struct motionParameters
  {
    const char * speedValue;
    int currentPin;
    int directionPin;
    int directionValue;
    int axisOnPin;
    int axisOffPin;
    int clockPin;
    int clockDelay;
    int limiterPin;
  };

 Propeller chip;
 double currentPosition[4];
 motionParameters stepperParameters[4];

 int Stepper_Power;

 void Stepper_Initialize();
 void Stepper_POWER_ON();
 void Stepper_POWER_OFF();
};

#endif
