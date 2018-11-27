#include "Arduino.h"
#include "WIFIBaby.h"

WIFIBaby::WIFIBaby(char* ssid, char* password, String apiKey){
	strcpy(_ssid, ssid);
	strcpy(_password, password);
	_apiKey = apiKey;
}

void WIFIBaby::connectWIFI(){
	WiFi.begin(_ssid, _password);


	while(WiFi.status() != WL_CONNECTED){
		delay(500);
		Serial.println(".");
	}
	
	Serial.println("");
	Serial.print("Conectado na rede ");
	Serial.println(_ssid);
	Serial.print("IP: ");
	Serial.println(WiFi.localIP());
}

void WIFIBaby::sendData(WiFiClient client, int BPM, float accelZ, float temp){
	if (client.connect(server,80)) {
		String postStr = _apiKey;
		postStr +="&amp;field1=";
		postStr += String(BPM);
		postStr +="&amp;field2=";
		postStr += String(accelZ);
		postStr +="&amp;field3=";
		postStr += String(temp);
		postStr += "\r\n\r\n";

	 	client.print("POST /update HTTP/1.1\n");
		client.print("Host: api.thingspeak.com\n");
		client.print("Connection: close\n");
		client.print("X-THINGSPEAKAPIKEY: "+_apiKey+"\n");
		client.print("Content-Type: application/x-www-form-urlencoded\n");
		client.print("Content-Length: ");
		client.print(postStr.length());
		client.print("\n\n");
		client.print(postStr);

		Serial.print("Frequência C: ");
		Serial.print(BPM);
		Serial.print(" Posicao: ");
		Serial.println(accelZ);
		Serial.print(" Temp: ");
		Serial.println(temp);

		client.stop();
	}
}