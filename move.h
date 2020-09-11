

#include "Arduino.h"



class move
{
public:

move();

int deltaT=500;//change this to alter how motor runs on each direction. 
int motorT_l=0;
int motorT_r=0;
int motorT_l_temp=0;
int motorT_r_temp=0;


int frame[3]={0,1,2};//ID of lighthouse(1,2,3)(O,X,Y)
float lighthouse_dist=1;//postion of lighthouse

float globalData[3]={0,0,0};//distance to (O,X,Y)
float globalPos[2]={0,0};//position(x,y)


void record();
void restore();
void takeMove(int Time);
void goStraight(int T);
void rotate(int Direct, int T);
void curve(int Direct, int A,int T);
//void getGlobal();


private:
const int ledPin = 13; 
const int motor_l=9;
const int motor_r=11;

};