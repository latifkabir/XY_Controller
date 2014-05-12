#include "propeller.h"
#include "constants.h"
#include <iostream>
#include <string>
#include <sstream>
#include <ctype.h>
#include<cstdlib>
#include <stdio.h>
#include <unistd.h>
#include <sys/io.h>

#include <fcntl.h>
#include <linux/ppdev.h> 
#include <linux/parport.h> 

#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/times.h> 
#include <sys/types.h> 
#include <sys/unistd.h> 

#include<signal.h> 


using namespace std;

Propeller::~Propeller()
{
 ioctl(fd, PPRELEASE);
 close(fd);
}

Propeller::Propeller()
{
	fd=open("/dev/parport0",O_RDWR);
	
	if(fd==-1)
	{
		cout<<"Unable to access the parallel port"<<endl;
		exit(1);
	}
	
	int result = ioctl(fd,PPCLAIM);
	

	for(int i=0; i<32; i++)
	{
		level[i]=0;
		dirty[i]=false;
	}
	clkfreq=80000000;
	cnt=0;
	
	writeLPT();
	readLPT();
}

void Propeller::outa(int pinNumber,int pinLevel)
{
	if(0<=pinNumber && pinNumber<32)
	{
		level[pinNumber] = pinLevel;
		dirty[pinNumber] = true;
	}
	writeLPT();
}

void Propeller::outa(int leftPin,int rightPin,int pinLevel)
{
	for(int pinNumber=max(rightPin,leftPin); pinNumber>=min(leftPin, rightPin); pinNumber--)
	{
		if(0<=pinNumber && pinNumber<32)
		{
			level[pinNumber] = pinLevel;
			dirty[pinNumber] = true;
		}
	}
	writeLPT();
}

void Propeller::outa(int upperPin,int lowerPin,const char *pinLevel)
{
	for(int pinNumber=upperPin, i=0; pinNumber>=lowerPin; pinNumber--, i++)
	{
		if(0<=pinNumber && pinNumber<32)
		{
			level[pinNumber] = (pinLevel[i] == '1');
			dirty[pinNumber] = true;
		}
	}
	writeLPT();
}

int Propeller::ina(int pinNumber)
{
	if(0<=pinNumber && pinNumber<32)
	{
		readLPT();
		return level[pinNumber];
	}
	return -1;
}

void Propeller::waitcnt(int ticks)
{
	double uinterval = 1000000.0*ticks/(1.0*clkfreq);
	while(uinterval>0)
	{
   	  double d=min(uinterval, 1000000.0);
	  usleep(d);
	  uinterval-=d;
	}
}

//PRIVATE

unsigned char Propeller::packData()
{
	unsigned char data=0;
	for(int i=0; i<8; i++)
		data |=(level[i]<<i);
	return data;
}

unsigned char Propeller::packControl()
{
	unsigned char control=0;
	control = (!level[RZ_SEL_PIN]) + (level[POWER_PIN]<<2) + ((!level[XY_SEL_PIN])<<3);
	//Take Control-2 as it is.
	//Control-0, Control-1, and Control-3 are inverted.
	//http://en.wikipedia.org/wiki/Parallel_port
	return control;
}

void Propeller::readLPT()
{
	unsigned char packed;
	ioctl(fd, PPRSTATUS, &packed);
	//Status-7 is inverted.
	
	level[X_Left]= !(packed & (1<<7));         //This bit comes in inverted.
	level[Y_High]= packed & (1<<6);
	level[Q6_Mark]= packed & (1<<5);
	level[Y_Limit]= packed & (1<<4);
	level[X_Limit]= packed & (1<<3);
}

void Propeller::writeLPT()
{
	unsigned char data=packData();
	unsigned char control = packControl();
	ioctl(fd, PPWDATA, &data);
	ioctl(fd, PPWCONTROL, &control);
}
