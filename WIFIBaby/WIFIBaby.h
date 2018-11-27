#ifndef WIFIBABY_H
#define WIFIBABY_H

#include "Arduino.h"
#include <ESP8266WiFi.h>


class WIFIBaby{
public:
	WIFIBaby(char* ssid, char* password, String apiKey);
	void connectWIFI();
	void sendData(WiFiClient client, int BPM, float accelZ, float temp);

private:
	char _ssid[25];
	char _password[25];
	String _apiKey;
	const char* server = "api.thingspeak.com";
};


#endif