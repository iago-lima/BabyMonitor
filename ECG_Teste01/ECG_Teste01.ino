/******************************************************************************
Heart_Rate_Display.ino
Demo Program for AD8232 Heart Rate sensor.
Casey Kuhns @ SparkFun Electronics
6/27/2014
https://github.com/sparkfun/AD8232_Heart_Rate_Monitor
The AD8232 Heart Rate sensor is a low cost EKG/ECG sensor.  This example shows
how to create an ECG with real time display.  The display is using Processing.
This sketch is based heavily on the Graphing Tutorial provided in the Arduino
IDE. http://www.arduino.cc/en/Tutorial/Graph
Resources:
This program requires a Processing sketch to view the data in real time.
Development environment specifics:
  IDE: Arduino 1.0.5
  Hardware Platform: Arduino Pro 3.3V/8MHz
  AD8232 Heart Monitor Version: 1.0
This code is beerware. If you see me (or any other SparkFun employee) at the
local pub, and you've found our code helpful, please buy us a round!
Distributed as-is; no warranty is given.
******************************************************************************/
/* The original code was adapted by: Marianna de Pinho Severo. This is the adapted code.*/

#include<math.h> 

#define LOM 14 //D5 ESP8266
#define LOm 12 //D6 ESP8266

int threshold = 750;    //Threshold used to indicate a R interval.
int cont = 0;
int BPM = 0;            //Variable used to store the heart rate.
int oldTime = millis(); //Variable used to store the timestamp of the last detected peak.
bool belowThreshold = true;   ////Variable used to indicate if there was the reading of a R interval.

void setup() {
  // initialize the serial communication:
  Serial.begin(9600);
  pinMode(LOM, INPUT); // Setup for leads off detection LO +
  pinMode(LOm, INPUT); // Setup for leads off detection LO -

}

void loop() {

  if((digitalRead(LOM) == 1)||(digitalRead(LOm) == 1)){ //Verify if the leads are connected.
    Serial.println('!');    //if one of them is not connected, we can't read the sensor.
  
  }else{ // if both leads are connected.
    float readInt = analogRead(A0);  //Read the value of analog input 0, where the sensor is connected.
    Serial.println(readInt);
    /*if (readInt > threshold && belowThreshold == true){  // Verify if a peak (R interval) was detected.
        
        Serial.println(calculateBPM()); // If a peak was detected, the heart rate is calculated (in beats per minute).
        belowThreshold = false;   //This indicate that the signal is above the predetermined threshold.
    
    }else if(readInt < threshold) //If a peak wasn't detected, we only indicate that the signal is below the predetermined threshold.
      {
        belowThreshold = true;
      }*/
  }
  
  //Serial.println(analogRead(A0));
  delay(1); //Wait for a bit to keep serial data from saturating. This is an one milisecond delay
}

/****************************************************************************
 * FUNCTION NAME: calculateBPM() 
 * DESCRIPTION: This function calculates the heart rate using the interval 
 * between two peaks, considering the interval between two adjacent peaks 
 * as homogeneous.This is based on the explanation presented in: 
 * https://www.youtube.com/watch?v=kFfKe6XRqGg
 ****************************************************************************/
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

/****************************************************************************
 * FUNCTION NAME: calculateBPM2() 
 * DESCRIPTION: This function calculates the heart rate using the intervals
 * between three consecutive peaks, considering them irregular.This is based 
 * on the explanation presented in: 
 * https://www.youtube.com/watch?v=kFfKe6XRqGg
 ****************************************************************************/
int calculateBPM2(){

    if(cont == 3){  //It verifies if four peaks were detected, that is, three intervals ocurred.
      int newTime = millis(); //It gets the current time.
      int diffTime = newTime - oldTime; //It uses the old time to calculate the time interval between the last peak and the current peak.

      oldTime = newTime; //It updates the oldTime to save the moment of the new last peak.
    
      int nbigBox = round((float)diffTime/200.0);   //It calculates the amount of big boxes, which have a length of 0.2 seconds (200 milisseconds).
                                                 //And rounds the result for the closest whole number.
      
      if(nbigBox){ //If there is, at least, a bigBox
        BPM = 900/nbigBox;     //Calculates the heart rate, assuming that the intervals are irregular.
      
      }else{  //If there are no bigBoxes, we normalize the heart rate to zero.
        BPM = 0;
      
      }
     
      cont = 0;  //Reset the amount of detected intervals. 
      
   }else{ //If three intervals were not detected yet, we assume that the heart rate is zero.
    BPM = 0;
    
   }

    cont++; //It increments the number of detected intervals.
    
    return BPM; //It returns the heart rate, in beats per minute.
}
