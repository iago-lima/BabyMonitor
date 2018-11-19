#include <ESP8266WiFi.h>
#include<math.h> 

extern "C"{
#include "user_interface.h"
}
 
os_timer_t mTimer;
os_timer_t mTimer2;

bool _timeout = false;
bool _timeout2 = false;

#define LOM 14 //D5 ESP8266
#define LOm 12 //D6 ESP8266

const char* ssid = "brisa-411523";
const char* password = "anvhlh5l";
 
String apiKey = "AVAPJV9CW4FIIDGU";      
const char* server = "api.thingspeak.com";

WiFiClient client;

int threshold = 750;    //Threshold used to indicate a R interval.
int cont = 0;
int BPM = 0;            //Variable used to store the heart rate.
int oldTime = millis(); //Variable used to store the timestamp of the last detected peak.
bool belowThreshold = true;   ////Variable used to indicate if there was the reading of a R interval.
int storeBPM = 0;

 
//Nunca execute nada na interrupcao, apenas setar flags!
void tCallback(void *tCall){
    _timeout = true;
}

void tCallback2(void *tCall){
    _timeout2 = true;
}

void usrInit(void){
    os_timer_setfn(&mTimer, tCallback, NULL);
    os_timer_arm(&mTimer, 1, true);

    os_timer_setfn(&mTimer2, tCallback2, NULL);
    os_timer_arm(&mTimer2, 60000, true);
}

void setup() {
  //Configuração da UART
  Serial.begin(9600);
  //Inicia o WiFi
  WiFi.begin(ssid, password);
 
  //Espera a conexão no router
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
 
  usrInit();

  pinMode(LOM, INPUT); // Setup for leads off detection LO +
  pinMode(LOm, INPUT); // Setup for leads off detection LO -
 
  //Logs na porta serial
  Serial.println("");
  Serial.print("Conectado na rede ");
  Serial.println(ssid);
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());
}
 
void loop() {

  if (_timeout){  
    getECG();
    _timeout = false;
  }

  if (_timeout2){ 
    //Inicia um client TCP para o envio dos dados
    if (client.connect(server,80)) {
      String postStr = apiKey;
             postStr +="&amp;field1=";
             postStr += String(storeBPM);
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
   
       //Logs na porta serial
       Serial.print("Heart Rate: ");
       Serial.println(storeBPM);
       storeBPM = 0;
    }
    client.stop(); 
    _timeout2 = false;
  }
  yield();
}

void getECG(){
   if((digitalRead(LOM) == 1)||(digitalRead(LOm) == 1)){
      Serial.println('!');    
  
     }else{ // if both leads are connected.
      float readInt = analogRead(A0);  
      
      if (readInt > threshold && belowThreshold == true){  
          storeBPM = calculateBPM();
          Serial.println(storeBPM); 
          belowThreshold = false;   
      
      }else if(readInt < threshold) 
        {
          belowThreshold = true;
        }
    }
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


