#include "Arduino.h"
#include "move.h"


move::move(){
  
}


void move::record(){
  motorT_l_temp=motorT_l;
  motorT_r_temp=motorT_r;
  }

void move::restore(){
  motorT_l=motorT_l_temp;
  motorT_r=motorT_r_temp;
}

void move::takeMove(int Time){
 int cnt=0;
 int cnt_limit=Time/(motorT_l+motorT_r);
 while(cnt<=cnt_limit){
 digitalWrite(motor_l,HIGH);
 delay(motorT_l);
 digitalWrite(motor_l,LOW);
 digitalWrite(motor_r,HIGH);
 delay(motorT_r);
 digitalWrite(motor_r,LOW);
 cnt=cnt+1;
 }
}

// T is runtime
void move::goStraight(int T){
  record();
  motorT_l=deltaT;
  motorT_r=motorT_l;
  takeMove(T);
}

//direct==1 or 0, T == run time
void move::rotate(int Direct, int T){
record();
motorT_l=deltaT*Direct;
motorT_r=deltaT-deltaT*Direct;
takeMove(T);
}
//The following can be improved later by getting data from the actual robot testing to see how to make a turn of certain radius
//driect == 1 or 0, A is turning constant, larger A gives curve with larger curvature, T s runtime
void move::curve(int Direct, int A,int T){
  record();
  if(Direct==1){
  motorT_l=deltaT;
  motorT_r=A*motorT_l;
  }
  else{ 
  motorT_r=deltaT;
  motorT_l=A*motorT_r;

  }
  takeMove(T);
}

/*
void move::getGlobal(){
  for(byte i=0;i<3;i=i+1){
    globalData[i]=getDist(frame[i]);
  }
  globalPos[0]=(lighthouse_dist*lighthouse_dist+globalData[0]*globalData[0]-globalData[1]*globalData[1])/(2*lighthouse_dist);
  float y=sqrt(globalData[0]*globalData[0]-globalPos[0]*globalPos[0]);
  float y_3=sqrt(globalData[2]*globalData[2]-globalPos[0]*globalPos[0])+lighthouse_dist;
  globalPos[1]=min(min(abs(y-y_3),abs(-y-y_3)),min(abs(y+y_3),abs(-y+y_3)));

}
*/
