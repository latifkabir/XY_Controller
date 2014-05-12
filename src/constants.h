#ifndef CONSTANTS_H
#define CONSTANTS_H
 #define TRUE  1
 #define FALSE 0

 //'Pin Assignments
 #define RZ_SEL_PIN  16
 #define POWER_PIN  18
 #define XY_SEL_PIN  19

 //'Motor Directions
 #define LEFT  1 //    'X
 #define RIGHT  0
 #define UP  1 //      'Y
 #define DOWN  0
 #define IN  1 //      'Z
 #define OUT  0 //
 #define CW  1 //      'R
 #define CCW  0

 //'Pin State Definitions
 //'---------------------

 //'Pin Logic Level
 #define HIGH  1
 #define LOW  0

 //'A7 Power Pin
 #define OFF  1
 #define ON  0

 //'I/O Status
 #define INPUT  0
 #define OUTPUT  1

 //'Data Bus Pin Assignments
 //'------------------------
 #define A5  0
 #define A6  1
 
 #define Yclk  2
 #define Zclk  2

 #define Ydir  3
 #define Zdir  3

 #define Xclk  4
 #define Rclk  4

 #define Xdir  5
 #define Rdir  5

 #define X_A4  6
 #define R_A4  6

 #define Y_A4  7
 #define Z_A4  7

 //'Limit Switch Pin Assignments
 //'----------------------------
 #define X_Limit  11
 #define Y_Limit  12
 #define Q6_Mark  13
 #define Y_High  14
 #define X_Left  15

 //'Current Levels to Motor Windings
 #define FULL_I  1
 #define HALF_I  0

 //'XY Speed Settings
 #define XY_OFF         "00" //%00
 #define XY_SLOW_SPEED  "01" //%01
 #define XY_MED_SPEED   "10" //%10
 #define XY_FAST_SPEED  "11" //%11

 #define X_SPEED  XY_FAST_SPEED
 #define Y_SPEED  XY_MED_SPEED

 //'RZ Speed Settings   (applies only when A4  1, with A71)
 #define RZ_OFF         "11" //%11
 #define RZ_SLOW_SPEED  "00" //%00
 #define RZ_MED_SPEED   "01" //%01
 #define RZ_FAST_SPEED  "10" //%10

 #define R_SPEED  RZ_MED_SPEED
 #define Z_SPEED  RZ_MED_SPEED
 
 #define DELAY_1SEC  80000000
 #define DELAY_2SEC  160000000
 #define DELAY_3SEC  240000000

 #define X_CLKS_PER_INCH  3200
 #define Y_CLKS_PER_INCH  3200
 #define Z_CLKS_PER_INCH  1000
 #define R_CLKS_PER_ROTATION  2400
#endif
