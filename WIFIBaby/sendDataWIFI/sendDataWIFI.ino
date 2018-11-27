#include<WIFIBaby.h>

WIFIBaby wifiBaby("brisa-411523", "anvhlh5l", "AVAPJV9CW4FIIDGU");
WiFiClient client;

void setup() {
  Serial.begin(9600);
  wifiBaby.connectWIFI();
}

void loop() {
  delay(2000);
  wifiBaby.sendData(client, 80, 88.6, 35);
}
