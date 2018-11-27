#include <ECG.h>
#include <TimerBabyM.h>
#include <WIFIBaby.h>
#include <AccelBaby.h>
#include <Thermistor.h>

ECG ecg(14,12);
TimerBabyM timerbaby;
AccelBaby mpu(2,0); //D4 e D3;
WIFIBaby wifiBaby("#resistencia", "elenunca", "AVAPJV9CW4FIIDGU");
WiFiClient client;
Thermistor temp(0);

float temperature, newTemp;

void tCallbackECG(void *tCall){
  timerbaby._timeoutECG = true;
}

void tCallbackACCEL(void *tCall){
 timerbaby._timeoutACCEL = true;
}

void tCallbackWIFI(void *tCall){
 timerbaby._timeoutWIFI = true;
}

AxisMPU accel;  

float zAngle = 0.0;

int ecgValue = 0, newEcg = 0.0;


void setup() {
  Serial.begin(9600);
  timerbaby.usrInit(&tCallbackECG,&tCallbackACCEL, &tCallbackWIFI);
  mpu.initI2C();
  mpu.initMPU(0,0);
  mpu.checkMPU(mpu.MPU_ADDR);
  
  wifiBaby.connectWIFI();
  
}

void loop() {
  
  if(timerbaby._timeoutECG){ 
    newEcg = ecg.getECG();

    if( newEcg != -1){
        ecgValue = newEcg;
    }
    
    //Serial.print("ECG: ");Serial.println(ecgValue);
    timerbaby._timeoutECG = false;
  }

  if(timerbaby._timeoutACCEL){
     accel = mpu.readRawACCEL();
     zAngle = mpu.getAngleAccelZ(accel.X, accel.Y, accel.Z);
    // Serial.print(" ACCEL: ");Serial.println(zAngle);

     delay(20);
     newTemp = readLine().toFloat();
      if( newTemp != -1.0){
        temperature = newTemp;
      }
    //Serial.print(" TEMP: ");Serial.println(temperature);
     
     timerbaby._timeoutACCEL = false;
  }
  
   if(timerbaby._timeoutWIFI){
     
     wifiBaby.sendData(client, ecgValue, zAngle, temperature); //Teste com valores aleatórios
     timerbaby._timeoutWIFI = false;
  }
  yield();
}

String line = "";
String readLine(){
  line = "";
  
  if(Serial.available() > 0){
    char c = Serial.read();
    while(c != '\n'){
      if(Serial.available() > 0){
        line += c;
        c = Serial.read();  
      }
    }
    c = '\0';
    line += '\n';
    return line;    
  }else{
    return "-1.0";  
  }
}
