#ifndef PROPELLER_H
#define PROPELLER_H

#include "constants.h"

class Propeller
{
  int level[32];
  bool dirty[32];
  unsigned char packData();
  unsigned char packControl();
  void readLPT();
  void writeLPT();
  void OLDwriteLPT();

public:
 int clkfreq;
 int cnt;
 int fd;

 Propeller();
 Propeller(int BASEPORT);
 ~Propeller();
 void setLPT(int BASEPORT);
 void outa(int pinNumber,int pinLevel);
 void outa(int leftPin,int rightPin,int pinLevel);
 void outa(int upperPin,int lowerPin,const char *pinLevel);
 int ina(int pinNumber);
 void waitcnt(int ticks);
};

#endif
