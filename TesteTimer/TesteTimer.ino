extern "C"{
#include "user_interface.h"
}
 
os_timer_t mTimer;
os_timer_t mTimer2;

#include<math.h> 

bool _timeout = false;
bool _timeout2 = false;

#define LOM 14 //D5 ESP8266
#define LOm 12 //D6 ESP8266

int threshold = 750;    //Threshold used to indicate a R interval.
int cont = 0;
int BPM = 0;            //Variable used to store the heart rate.
int oldTime = millis(); //Variable used to store the timestamp of the last detected peak.
bool belowThreshold = true;   ////Variable used to indicate if there was the reading of a R interval.

 
//Nunca execute nada na interrupcao, apenas setar flags!
void tCallback(void *tCall){
    _timeout = true;
}

void tCallback2(void *tCall){
    _timeout2 = true;
}
void usrInit(void){
    os_timer_setfn(&mTimer, tCallback, NULL);
    os_timer_arm(&mTimer, 10, true);

     os_timer_setfn(&mTimer2, tCallback2, NULL);
    os_timer_arm(&mTimer2, 10, true);
}

 
void setup() {
  Serial.begin(9600);
  Serial.println();
  //iniciar a interrupcao
  usrInit();

  pinMode(LOM, INPUT); // Setup for leads off detection LO +
  pinMode(LOm, INPUT); // Setup for leads off detection LO -
 
}
void getECG(){
   if((digitalRead(LOM) == 1)||(digitalRead(LOm) == 1)){
      Serial.println('!');    
  
     }else{ // if both leads are connected.
      float readInt = analogRead(A0);  
      
      if (readInt > threshold && belowThreshold == true){  
          
          Serial.println(calculateBPM()); 
          belowThreshold = false;   
      
      }else if(readInt < threshold) 
        {
          belowThreshold = true;
        }
    }
}
 
void loop() {
  //verificacao no loop
  if (_timeout){  
    getECG();
    _timeout = false;
  }

  if (_timeout2){  
    Serial.println("Aqui");
    _timeout2 = false;
  }
  yield();  
}

int calculateBPM(){
  
    int newTime = millis(); //It gets the current time.
    int diffTime = newTime - oldTime; //It uses the old time to calculate the time interval between the last peak and the current peak.
    
    oldTime = newTime;  //It updates the oldTime to save the moment of the new last peak.
    
    int nbigBox = round((float)diffTime/200.0);  //It calculates the amount of big boxes, which have a length of 0.2 seconds (200 milisseconds).
                                                 //And rounds the result for the closest whole number.
    if(nbigBox){  //If there is, at least, a bigBox
      BPM = 300/nbigBox;      //Calculates the heart rate, assuming that the intervals are homogeneous.
     
    }else{    //If there are no bigBoxes, we normalize the heart rate to zero.
      BPM = 0;
    
    }
     
   return BPM;  //Return the heart rate, in beats per minute.
}
