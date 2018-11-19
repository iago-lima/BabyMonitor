#include <ECG.h>

ECG ecg(14,12);

void setup() {
  Serial.begin(9600);
  
}

void loop() {
  ecg.getECG();
  yield();
}
