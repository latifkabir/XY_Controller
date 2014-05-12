#include "spotseeker.h"
#include "constants.h"
#include <iostream>
#include <string>
#include <sstream>
#include <ctype.h>

#include <stdio.h>
#include <unistd.h>
#include <sys/io.h>
#include "math.h"

using namespace std;

SpotSeeker::SpotSeeker()
{
 Stepper_Initialize();
 Stepper_POWER_ON();
}

SpotSeeker::~SpotSeeker()
{
 Stepper_POWER_OFF();
}

int SpotSeeker::moveLink(int axis, int cycles, const bool steady)
{
	int & cnt=chip.cnt;
	int cycles_completed=0;
	int stepdir;

	if (!Stepper_Power)
		return cycles_completed;

	//'select stepping sequence
	chip.outa(A6,A5, stepperParameters[axis].speedValue);

	//'raise motor winding current
	chip.outa(stepperParameters[axis].currentPin, FULL_I);

	//'select direction of movement
	chip.outa(stepperParameters[axis].directionPin, stepperParameters[axis].directionValue);
	stepdir = 1;
	if (cycles < 0)
	{
		cycles= -cycles;
		stepdir=-stepdir;
		chip.outa(stepperParameters[axis].directionPin, !stepperParameters[axis].directionValue);
	}
	//'enable XY motor control
	chip.outa(stepperParameters[axis].axisOnPin, HIGH);	
	chip.outa(stepperParameters[axis].axisOffPin, LOW);

	//For constant acceleration, v(x) = a*sqrt(2*x/a)
	double a=0.001;
	double v0=0.0;
	double x=.001;
	double v=1.0;
	//TODO: Decelerate at the tail end.

	//'pulse stepper motor
	for(int i=0; i<cycles; i++)
	{
		if(stepperParameters[axis].limiterPin >=0)
			if (chip.ina(stepperParameters[axis].limiterPin))
			{
				cout<<"Hit Limit"<<endl;
				break;
			}
	
		if(steady)
		{
			v=a*sqrt(2.0*(x)/fabs(a))+v0;
			x++;
			if(i==cycles/2)
			{
				v0=v;
				a=-a;
				cout<<v0<<" "<<a<<endl;
				x=0;
			}
			cout<<v<<endl;
		}
			
		chip.outa(stepperParameters[axis].clockPin, HIGH);
		chip.waitcnt(stepperParameters[axis].clockDelay/v + cnt);
		chip.outa(stepperParameters[axis].clockPin, LOW);
		chip.waitcnt(stepperParameters[axis].clockDelay/v + cnt);

		cycles_completed += 1;

		if (stepdir < 0)
			currentPosition[axis] -= 1;
		else
			currentPosition[axis] += 1;
	}
	
	//'lower motor winding current
	chip.outa(stepperParameters[axis].currentPin, HALF_I);

	//'disable XY motor control
	chip.outa(stepperParameters[axis].axisOnPin, LOW);

	return cycles_completed;
}

int SpotSeeker::moveX(int clicks)
{
	return moveLink(0,clicks,false);
}

int SpotSeeker::moveY(int clicks)
{
	return moveLink(1,clicks,false);
}

int SpotSeeker::moveZ(int clicks)
{
	return moveLink(2,clicks,false);
}

int SpotSeeker::moveR(int clicks)
{
	return moveLink(3,clicks,false);
}

Vector4 SpotSeeker::moveTo(double x, double y, double z)
{
	Vector4 returnValue={0,0,0,0};
	returnValue.x=moveX(x-currentPosition[0]);
	returnValue.y=moveY(y-currentPosition[1]);
	returnValue.z=moveZ(z-currentPosition[2]);
}

Vector4 SpotSeeker::getPosition()
{
	Vector4 returnValue = {currentPosition[0], currentPosition[1], currentPosition[2], currentPosition[3]};
	return returnValue;
}

void SpotSeeker::setOrigin()
{
  currentPosition[0]=0;
  currentPosition[1]=0;
  currentPosition[2]=0;
  currentPosition[3]=0;
}

void SpotSeeker::performDemo()
{
	int &cnt = chip.cnt;
	Stepper_Initialize();

	Stepper_POWER_ON();

	//repeat 2
	for(int i=0; i<2; i++)
	{

		//'14.5" RANGE OF X MOTION
		//repeat 1
		for(int j=0; j<1; j++)
		{
			moveX(5 * X_CLKS_PER_INCH);
			chip.waitcnt(DELAY_3SEC + cnt);
			moveX(-5 * X_CLKS_PER_INCH);
			chip.waitcnt(DELAY_3SEC + cnt);
		}

		//'17.125" RANGE OF Y MOTION
		//repeat 1
		for(int j=0; j<1; j++)
		{
			moveY(20 * Y_CLKS_PER_INCH);
			chip.waitcnt(DELAY_2SEC + cnt);
			moveY(-20 * Y_CLKS_PER_INCH);
			chip.waitcnt(DELAY_2SEC + cnt);
		}
		//'3" RANGE OF Z MOTION
		//repeat 1
		for(int j=0; j<1; j++)
		{
			moveZ(1000); //'(2500)
			chip.waitcnt(DELAY_1SEC + cnt);
			moveZ(-1000); //'(-2500)
			chip.waitcnt(DELAY_1SEC + cnt);
		}
		//'0.15 DEGS PER CLOCK, 600 CLOCKS = 90 DEGS ROTATION
		//repeat 1
		for(int j=0; j<1; j++)
		{
			moveR(500);
			chip.waitcnt(DELAY_1SEC + cnt);
			moveR(-500);
			chip.waitcnt(DELAY_1SEC + cnt);
		}
	}
	Stepper_POWER_OFF();
}

//PRIVATES:
void SpotSeeker::Stepper_Initialize()
{
	int XDELAY = 25000;
	int YDELAY = 25000;
	int ZDELAY = 100000;
	int RDELAY = 30000;  
	
	currentPosition[0] = 0;
	currentPosition[1] = 0;
	currentPosition[2] = 0;
	currentPosition[3] = 0;
	
	motionParameters mX=  {X_SPEED, X_A4, Xdir, RIGHT, XY_SEL_PIN, RZ_SEL_PIN, Xclk, XDELAY, X_Limit};
	motionParameters mY = {Y_SPEED, Y_A4, Ydir, UP   , XY_SEL_PIN, RZ_SEL_PIN, Yclk, YDELAY, Y_Limit};
  	motionParameters mZ = {Z_SPEED, Z_A4, Zdir, IN   , RZ_SEL_PIN, XY_SEL_PIN, Zclk, ZDELAY, -1};
	motionParameters mR = {R_SPEED, R_A4, Rdir, CW   , RZ_SEL_PIN, XY_SEL_PIN, Rclk, RDELAY, -1};
	stepperParameters[0]=mX;
	stepperParameters[1]=mY;
	stepperParameters[2]=mZ;
	stepperParameters[3]=mR;


/*
    'b7 b6      b5 b4       b3 b2       b1 b0
    '-----------------------------------------
    'Z  R         R           Z          R&Z

    'Y  X         X           Y          X&Y
    '1 =driveI  dir cnt     dir cnt     speed
    '0 =holdI
 */
	//'INPUTS
	//dira[15..11] := %00000      'status[7:3] defined as input pins

	//'DATA BUS
	chip.outa(7,0, LOW); //          'data[7:0] default state ...all zeros
	//dira[7..0] := %1111_1111    'data[7:0] defined as output pins


	//'POWER
	chip.outa(POWER_PIN, OFF); //      'control[2] default state ...high
	//dira[POWER_PIN] := OUTPUT   'control[2] defined as output pin

	//'RZ_ENABLE
	chip.outa(RZ_SEL_PIN, LOW); //     'control[0] default state ...low
	//dira[RZ_SEL_PIN] := OUTPUT  'control[0] defined as output pin

	//'XY_ENABLE
	chip.outa(XY_SEL_PIN, LOW); //     'control[3] default state ...low
	//dira[XY_SEL_PIN] := OUTPUT  'control[3] defined as output pin

	//'Write default value to RZ Register
	//'----------------------------------
	chip.outa(7,0, 0); //             'default A4=0 (holding-I) for both motors
	chip.outa(RZ_SEL_PIN, HIGH);
	chip.outa(RZ_SEL_PIN, LOW); //     'active-high pulse to latch RZ register data

	//'Write default value to XY Register
	//'----------------------------------
	chip.outa(7,0,0); //             'default A4=0 (holding-I) for both motors
	usleep(5000);
	chip.outa(XY_SEL_PIN, HIGH);
	usleep(5000);
	chip.outa(XY_SEL_PIN, LOW); //     'active-high pulse to latch XY register data
	Stepper_Power = FALSE;
}

void SpotSeeker::Stepper_POWER_ON()
{
	//'@@@@@@@@@@ Enable Steppers @@@@@@@@@@
	chip.outa(POWER_PIN, ON); //       'XY enabled, RZ enabled
	Stepper_Power = TRUE;
}

void SpotSeeker::Stepper_POWER_OFF()
{
	//'@@@@@@@@@@ Disable Steppers @@@@@@@@@@
	chip.outa(POWER_PIN, OFF); //      'XY disabled, RZ disabled
	Stepper_Power = FALSE;
}
