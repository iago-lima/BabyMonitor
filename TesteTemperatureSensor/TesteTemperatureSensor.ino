#include <Thermistor.h>

Thermistor temp(0);

void setup() {
  Serial.begin(9600);
}

void loop() {
  int temperature = temp.getTemp();
  Serial.print("Temperatura no Sensor é: ");
  Serial.print(temperature);
  Serial.println("*C");
  delay(1000);
}

 
