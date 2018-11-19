#include <ECG.h>
#include <TimerBabyM.h>

ECG ecg(14,12);
TimerBabyM timerbaby;


void tCallbackSensor(void *tCall){
  timerbaby._timeoutSensor = true;
}

void tCallbackWifi(void *tCall){
 timerbaby._timeoutWifi = true;
}


void setup() {
  Serial.begin(9600);
  timerbaby.usrInit(&tCallbackSensor,&tCallbackWifi);
  
}

void loop() {
  if (timerbaby._timeoutSensor){
      ecg.getECG();
      timerbaby._timeoutSensor = false;
  }
  yield();
}
