#include "Arduino.h"
#include "ECG.h"

ECG::ECG(int LOM, int LOm){
	 pinMode(_LOM, INPUT); // Setup for leads off detection LO +
 	 pinMode(_LOm, INPUT);

	_LOM = LOM;
	_LOm = LOm;
}

int ECG::getECG(){
	int storeBPM = 0;
	if((digitalRead(_LOM) == 1)||(digitalRead(_LOm) == 1)){
		Serial.println('!');    
  
     }else{ 
     
      float readInt = analogRead(A0);  
      if (readInt > _threshold && _belowThreshold == true){  
          storeBPM = calculateBPM();
          Serial.println(storeBPM); 
          _belowThreshold = false;   
      
      }else if(readInt < _threshold) 
        {
          _belowThreshold = true;
        }
    }
    return storeBPM;
}

int ECG::calculateBPM(){
	int BPM = 0;
	int newTime = millis(); //It gets the current time.
    int diffTime = newTime - _oldTime; //It uses the old time to calculate the time interval between the last peak and the current peak.
    
    _oldTime = newTime;  //It updates the oldTime to save the moment of the new last peak.
    
    int nbigBox = round((float)diffTime/200.0);  //It calculates the amount of big boxes, which have a length of 0.2 seconds (200 milisseconds).
                                                 //And rounds the result for the closest whole number.
    if(nbigBox){  //If there is, at least, a bigBox
      BPM = 300/nbigBox;      //Calculates the heart rate, assuming that the intervals are homogeneous.
     
    }else{    //If there are no bigBoxes, we normalize the heart rate to zero.
      BPM = -1;
    
    }
     
   return BPM;  //Return the heart rate, in beats per minute.
}