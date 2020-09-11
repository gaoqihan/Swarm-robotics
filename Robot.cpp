#include "Arduino.h"
#include "Robot.h"
#include "IRremote.h"
#include "move.h"

//id is a 8 bit number greater than 0x10


IRrecv irrecv(11);
IRsend irsend;
decode_results results;


Robot::Robot(int myID){

    if (myID>=0x90){
    _myID=myID;
    _header=myID<<8;
    Serial.println("id correct");
    }
    else{
    Serial.begin(9600);
    Serial.println("ID must be in format of 0x##>=0x90");
    }
}

void Robot::initial_Setup(){
    irrecv.enableIRIn();
    Serial.println("ir interupt intialized");
    }

void Robot::reach_connect(int target,int currentID_mem){
    int connected=0;
    int cnt=0;
    while(connected !=1){
        if(cnt>=1000){
            Serial.println("not connected");
            break;
            }
        cnt++;
        Serial.print(cnt);
        Serial.println("    connecting");
        Serial.println(target<<8|_myID,BIN);

       irsend.sendSony(target<<8|_myID,16);
       irrecv.enableIRIn();

       delay(50);
       Serial.println(((_myID<<8)|target),BIN);//will delete
         if(irrecv.decode(&results)) {
             Serial.print("got result ");
             Serial.println(results.value,BIN);
             Serial.println(results.value&0xff,BIN);


             if (results.value==((_myID<<8)|target)){
                Serial.print("mate ID:");
                Serial.println(results.value&0xff,HEX);
                mateID=target;
                connected=1;
                _currentID_mem=currentID_mem;
                Serial.println("connected");
                Serial.println("sending reply to mate");

                sendInfo_mult(0x2,50);
             }
             irrecv.resume();
        }
    }
}

void Robot::recv_connect(int currentID_mem){
    mateID=0;
    int cnt=0;
    int connected=0;
    while(connected!=1){
            if(cnt>=1000){
            Serial.println("not connected");
            break;
            }
        cnt++;
        Serial.print(cnt);
        Serial.println("connecting");
        if(irrecv.decode(&results)) {
            delay(100);
            Serial.print("got result");
            Serial.println(results.value,BIN);
            Serial.println(results.value>>8,BIN);
            Serial.println(_myID,BIN);
            if (results.value==(_myID<<8|0x2)){
                Serial.println(" that my mate have my ID");
                
                connected=1;
            }
            else if (results.value>>8==_myID){
                Serial.print(" with my ID:");
                Serial.println(results.value,BIN);
                mateID=results.value&0xff;
                _currentID_mem=currentID_mem;
            }

        irrecv.resume();
   
        }
        if((mateID!=0)&(connected!=1)){
            irsend.sendSony(((mateID<<8)|_myID),16);
            irrecv.enableIRIn();

            delay(50);
            Serial.print(" Sending my ID");
            Serial.println((mateID<<8)|_myID,BIN);

            
        }
    }
}
  
void Robot::sendInfo(int info){
    int received=0;
    while(received!=1){
        irsend.sendSony(mateID<<8|info,16);
        Serial.print("sending:");
        Serial.println(mateID<<8|info);
        if(irrecv.decode(&results)) {
            if (results.value==_header|0x1){
                received=1;
                Serial.println("info sent successfully");
                irrecv.resume();
                }
        }
    }
}

void Robot::sendInfo_mult(int info,int cnt){//not expecting respond
    for(int i=0;i<cnt;i++){
        irsend.sendSony(mateID<<8|info,16);
        Serial.print("sending mult info");
        Serial.println(mateID<<8|info,HEX);
        Serial.print("Mate ID");
        Serial.println(mateID,HEX);
        irrecv.enableIRIn();

        delay(50);
    }
}


void Robot::broadcast(int info){//send to everyone
    for(int i=0;i<100;i++){
        irsend.sendSony(0xff<<8|info,16);
        delay(50);
    }
void Robot::broadcast_near(int info){//send to everyone
    for(int i=0;i<100;i++){
        irsend.sendSony(0xee<<8|info,16);
        delay(50);
    }


void Robot::running(){ //put this function in loop()
    if(irrecv.decode(&results)) {
        if (results.value>>8==0xff){
        if((results.value>>8==_myID)||(results.value>>8==0xff)||(results.value>>8==0xee)){
            Serial.print("Received info");
            Serial.println(results.value&0xff,HEX);
            if(results.value>>8==0xff){//if receive broadcasted info, broad cast it too.
            broadcast(results.value&0xff);
            }
            
            if((results.value&0xff)==0x3){//lights up
                digitalWrite(_ledPin,HIGH);
                sendInfo_mult(0x1,20);
                irrecv.resume();

            }
            
            else if((results.value&0xff)==0x4){//lights off
                digitalWrite(_ledPin,LOW);
                Serial.println("okok");
                sendInfo_mult(0x1,20);
                irrecv.resume();
            }
            
            else if((results.value&0xff)==0x5){//reach face towards
                sendInfo_mult(0x01，50);
                face_towards();
            }
            
        }
    irrecv.resume();

    }
}
    
float Robot::distance_average(){
    float distance[3][3];
    for(int i=0;i<1000;i++){
        int x=analogRead(_analog_receive_Pin_2);
        distance[0][0]=distance[0][0]+analogRead(_analog_receive_Pin_0);
        distance[1][0]=distance[1][0]+analogRead(_analog_receive_Pin_1);
        distance[2][0]=distance[2][0]+x;
                                        Serial.print(x);
                                        Serial.print("    sum:");

                                Serial.println(distance[2][0]);

    }
    distance[0][1]=distance[0][0]/1000;
    distance[1][1]=distance[1][0]/1000;
    distance[2][1]=distance[2][0]/1000;
                                    Serial.println(distance[2][1]);

    float output=max(max(distance[0][1],distance[1][1]),max(distance[2][1],100));
    if(output==100){
        return 0;
    }
    else{
    return output;
    }
}
       
void Robot::getdist(){
    sendInfo_mult(0x3,50);
    delay(1000);
    if(irrecv.decode(&results)) {
        Serial.println(results.value,BIN);
        if(results.value>>8==_myID){
            Serial.println("it's for me");
                        Serial.println(results.value&0xff);

            if((results.value&0xff)==0x1){//lights ready
                Serial.println("measure dist");
                dist[_currentID_mem][0]=mateID;
                dist[_currentID_mem][1]=distance_average();
                Serial.println(dist[_currentID_mem][1]);
            }
        }
    irrecv.resume();

    }
    sendInfo_mult(0x4,50);
    }
    
    
int Robot::get_direction(){
    broadcast(0x4);//all turn off light
    delay(1000);
    sendInfo_mult(0x3,50);
    int output;
    int temp_max=0;
    float distance[3][3];
    for(int i=0;i<1000;i++){
        distance[0][0]=distance[0][0]+analogRead(_analog_receive_Pin_0);
        distance[1][0]=distance[1][0]+analogRead(_analog_receive_Pin_1);
        distance[2][0]=distance[2][0]+analogRead(_analog_receive_Pin_2);
    }
    distance[0][1]=distance[0][0]/1000;
    distance[1][1]=distance[1][0]/1000;
    distance[2][1]=distance[2][0]/1000;
    
    for(i=0;i<3;i++){
        if(distance[i][1]>temp_max){
            output=i;
        }
    }
return output;
}

int Robot::get_front(){
    int temp_sum=0;
    int front_reading=0;
    for(int i=0;i<1000;i++){
       temp_sum=temp_sum+analogRead(_analog_receive_Pin_0);
    }
    front_reading=temp_sum/1000;
    return front_reading;
    }


void Robot::face_towards(){
    int delta_time=full_round_time/16;
    int cnt=0;
    int distance_holder[16];
    int max_cnt;
    int temp_max;
    bool facing=False;
    while(cnt<16){
        move.rotate(1,delta_time);
        distance_holder[cnt]=getfront();
        if(distance_holder[cnt]>temp_max){
            temp_max=distance_holder[cnt];
            max_cnt=cnt;
        }
        cnt=cnt+1;
    }
    for(int i=0;i<max_cnt;i++){
        move.rotate(1,delta_time);
        }
}

void Robot::face_backwards(){
    int delta_time=full_round_time/16;
    int cnt=0;
    int distance_holder[16];
    int max_cnt;
    int temp_max;
    bool facing=False;
    while(cnt<16){
        move.rotate(1,delta_time);
        distance_holder[cnt]=getfront();
        if(distance_holder[cnt]>temp_max){
            temp_max=distance_holder[cnt];
            max_cnt=cnt;
        }
        cnt=cnt+1;
    }
    for(int i=0;i<max_cnt;i++){
        move.rotate(1,delta_time);
        }
    for (int j=0;j<8;j++){
        move.rotate(1,delta_time);
        }

}

/*
void Robot::keep_distance(int distance){
    face_towards();
    int temp_dist=getdist();
    int diff=distance-temp_dist;
    
    if (diff>=0){
        move.Straight(diff/straight_v);
        }
    else{
        face_backwards();
        move.Straight((-1*diff)/straight_v);        
        }
    face_towards();
    temp_dist=getdist();
    diff=distance-temp_dist;
    if (diff>=50){
        keep_distance(distance);
        }
        
}
*/
    
void reach_face(){
    int delta_time=full_round_time/16;
    int cnt=0;
    sendInfo_mult(0x05，50);
    if(irrecv.decode(&results)){
        if (results.value==((_myID<<8)|0x01)){
            
            face_towards();
    }  
}



