
/*
readme
*/

#ifndef Robot_h
#define Robot_h

#include "Arduino.h"


class Robot
{
public:
    Robot(int myID);
    int full_round_time=10;//change this after measuring
    int straight_v=100//change this after measuring (unit 1/sec)
    int dist[4][3];//a space that holds distance to 4 robots next to me. change the # in first[] to change how many robot's distance to be stored. Theoritically knowing the position of 3 robots and distance to them can give you position of my own.
    //int Angle;
    int mateID;//ID of the current mate, 8 bit long.
    void initial_Setup();//put in setup(), enable the interrupt for receiving ir signal.
    void reach_connect(int target,int currentID_mem);//connect to a mate that has recv_connect running. If successfully connected, will store the id of the target into mateID; current ID mem is which 1-4 number in dist[4][3] you would like to recognize this robot as.
    void recv_connect(int currentID_mem);//connect to a mate that has reach_connect running. If successfully connected, will store the id of the target into mateID
    void sendInfo(int info);//DO NOT USE THIS; send 8 bit info in format of targetID+info (16 bit in total); This send info expect a respond of myID+0x1. otherwise will run forever
    void sendInfo_mult(int info,int cnt);// send 8 bit info in format of targetID+info (16 bit in total) for cnt times.
    void broadcast(int info);//send info to everyone and expect the info to be passed on to every robot in the swarm. the info is sent in format of 0xff+info
    void running();//a robot that is running running() will respond to command in format of  _myID+info or 0xff+info.
    //command list
    //0x1 received info 
    //0x2 received ID
    //0x3 turn on lights
    //0x4 turn off the lights
    void getdist();//ask the mate to turn on the light, measure the distance to the mate with the receivers, take the largest reading as distance to mate, then send signal to turn off the lights// 
    int get_direction()//ask mate to run on the lights, measure the readings of the receivers, output the number of receiver that have the largest reading
    int get_front()//get reading of the front receiver
    void face_towards()//face the front receiver towards the brightest lightsource
    void face_backwards()//face away form the brightest light source
    void keep_distance(int distance)//maintain a certain distance from mate. 
    




private:
    int _ledPin=3;
    int _analog_receive_Pin_0=A0;
    int _analog_receive_Pin_1=A1;
    int _analog_receive_Pin_2=A3;
    int IR_RECEIVE_PIN = 11;

    
    int _myID;
    int _header;//myID<<8
    int _command;
    int _currentID_mem;//serial number for where to store the curren mate's distance

    float distance_average();//get average of the readings

};

#endif