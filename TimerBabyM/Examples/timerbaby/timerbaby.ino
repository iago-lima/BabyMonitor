#include <TimerBabyM.h>

TimerBabyM timerbaby;


void tCallbackSensor(void *tCall){
  timerbaby._timeoutSensor = true;
}

void tCallbackWifi(void *tCall){
 timerbaby._timeoutWifi = true;
}


void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  timerbaby.usrInit(&tCallbackSensor,&tCallbackWifi);
}

void loop() {
 if (timerbaby._timeoutSensor){
  Serial.println("cuco!");
  timerbaby._timeoutSensor = false;
}
 
  yield();
}

