/* Oficina Maker IoT - EsalqShow
 
 Realização: Embarcados
 
*/

extern "C"{
#include "user_interface.h"
}
 
//Include da lib de Wifi do ESP8266
#include <ESP8266WiFi.h>
#include<math.h> 

#define LOM 14 //D5 ESP8266
#define LOm 12 //D6 ESP8266
 
//Definir o SSID da rede WiFi
const char* ssid = "brisa-411523";
//Definir a senha da rede WiFi
const char* password = "anvhlh5l";
 
//Colocar a API Key para escrita neste campo
//Ela é fornecida no canal que foi criado na aba API Keys
String apiKey = "WGSMNMTERBC8U0SD";       //trocar pela API Write
const char* server = "api.thingspeak.com";

int threshold = 750;    //Threshold used to indicate a R interval.
int cont = 0;
int BPM = 0;            //Variable used to store the heart rate.
int oldTime = millis(); //Variable used to store the timestamp of the last detected peak.
bool belowThreshold = true;   ////Variable used to indicate if there was the reading of a R interval.

 

WiFiClient client;
 
void setup() {
  pinMode(LOM, INPUT); // Setup for leads off detection LO +
  pinMode(LOm, INPUT); // Setup for leads off detection LO -
  
  //Configuração da UART
  Serial.begin(9600);
  //Inicia o WiFi
  WiFi.begin(ssid, password);
 
  //Espera a conexão no router
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
 
  //Logs na porta serial
  Serial.println("");
  Serial.print("Conectado na rede ");
  Serial.println(ssid);
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());
}

 int bpmA = 0;
void loop() { 
  
  if((digitalRead(LOM) == 1)||(digitalRead(LOm) == 1)){ //Verify if the leads are connected.
      Serial.println('!');    //if one of them is not connected, we can't read the sensor.
    
    }else{ // if both leads are connected.
      float readInt = analogRead(A0);  //Read the value of analog input 0, where the sensor is connected.
      
      if (readInt > threshold && belowThreshold == true){  // Verify if a peak (R interval) was detected.   
          bpmA = calculateBPM();
          Serial.println(bpmA); // If a peak was detected, the heart rate is calculated (in beats per minute).
          belowThreshold = false;   //This indicate that the signal is above the predetermined threshold.
      
      }else if(readInt < threshold) //If a peak wasn't detected, we only indicate that the signal is below the predetermined threshold.
        {
          belowThreshold = true;
        }
    }
 
  //Inicia um client TCP para o envio dos dados
 if(){
  if (client.connect(server,80)) {
    String postStr = apiKey;
           postStr +="&amp;field1=";
           postStr += String(bpmA);
           postStr += "\r\n\r\n";
 
     client.print("POST /update HTTP/1.1\n");
     client.print("Host: api.thingspeak.com\n");
     client.print("Connection: close\n");
     client.print("X-THINGSPEAKAPIKEY: "+apiKey+"\n");
     client.print("Content-Type: application/x-www-form-urlencoded\n");
     client.print("Content-Length: ");
     client.print(postStr.length());
     client.print("\n\n");
     client.print(postStr);
    }
  }
  client.stop();
 
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
