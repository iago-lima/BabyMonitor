extern "C"{
#include "user_interface.h"
}
 
os_timer_t mTimer;
os_timer_t mTimer2;

#include<math.h> 
#include <Wire.h> 

bool _timeout = false;
bool _timeout2 = false;

//#define LOM 14 //D5 ESP8266
//#define LOm 12 //D6 ESP8266

const int MPU_ADDR =      0x68; 
const int WHO_AM_I =      0x75; 
const int PWR_MGMT_1 =    0x6B; 
const int GYRO_CONFIG =   0x1B; 
const int ACCEL_CONFIG =  0x1C; 
const int ACCEL_XOUT =    0x3B;

const int sda_pin = D5; // Definition of I2C SDA pin.(GPIO 14 ESP8266)
const int scl_pin = D6; // Definition of I2C SCL.(GPIO 12 ESP8266)
 
//Variable to store the raw data from accelerometer. 
int16_t AcX, AcY, AcZ;

/*int threshold = 750;    //Threshold used to indicate a R interval.
int cont = 0;
int BPM = 0;            //Variable used to store the heart rate.
int oldTime = millis(); //Variable used to store the timestamp of the last detected peak.
bool belowThreshold = true;   ////Variable used to indicate if there was the reading of a R interval.
*/

void initI2C() 
{
  Wire.begin(sda_pin, scl_pin);
}

void writeRegMPU(int reg, int val){     //Accept a register and a value as parameters.
  
  Wire.beginTransmission(MPU_ADDR);     // It initiates the communication using the address of MPU6050.
  Wire.write(reg);                      // It sends the address of the register with which we want to communicate.
  Wire.write(val);                      // It sends the value we want to store into the register.
  Wire.endTransmission(true);           // It ends the transmission.
  
}

uint8_t readRegMPU(uint8_t reg){        //Accept a register as parameter.
  
  uint8_t data;                         //Variable used to store the value read.
  Wire.beginTransmission(MPU_ADDR);     // It initiates the communication using the address of MPU6050.
  Wire.write(reg);                      // It sends the address of the register with which we want to communicate.
  Wire.endTransmission(false);          // It ends the transmission, but keeps the I2C channel opened (sends STOP and START).
  Wire.requestFrom(MPU_ADDR, 1);        // It configures to receive 1 byte from the chosen register.
  data = Wire.read();                   // It reads the byte and stores into the data variable.
  return data;                          // It returns the data stored in data.
}

void findMPU(int mpu_addr){
  
  Wire.beginTransmission(mpu_addr); //It initiates the communication using the MPU6050 address.
  int data = Wire.endTransmission(true);  //It stops the communication and saves the result into the variable data.
 
  if(data == 0){    //If data == 0, the device was found.
    Serial.print("Device found at the address: 0x");
    Serial.println(mpu_addr, HEX);
    
  }else{  //If data != 0, the device was not found.
    Serial.println("Device wasn't found!");
  }
}

void checkMPU(int mpu_addr){
  findMPU(MPU_ADDR);  //It verifies if the device is connected.
   
  int data = readRegMPU(WHO_AM_I); // It reads the identification of the device. Register 117 – Who Am I - 0x75
   
  if(data == 104){
    
    Serial.println("MPU6050 device answered OK! (104)");
    data = readRegMPU(PWR_MGMT_1); // It reads the operation mode of the device. Register 107 – Power Management 1-0x6B
 
    if(data == 64){
      Serial.println("MPU6050 is in SLEEP mode! (64)");
      
    }else{
      Serial.println("MPU6050 is in ACTIVE mode!"); 
      
    }
    
  }else{
    Serial.println("Verify the device - MPU6050 is not available!");
  }
}

void initMPU(){
  setSleepOff();   //It activates the sensor
  setGyroScale();  //It configures the gyroscope scale.
  setAccelScale(); //It configures the accelerometer scale.
  
}

void setSleepOff(){
  
  writeRegMPU(PWR_MGMT_1, 0); // It writes zero into the energy management register.
  
}

void setGyroScale(){
  
  writeRegMPU(GYRO_CONFIG, 0);
  
}

void setAccelScale(){
  
  writeRegMPU(ACCEL_CONFIG, 0);

}

void readRawMPU()
{  
  Wire.beginTransmission(MPU_ADDR);       // It initiates the communication using the address of MPU6050.
  Wire.write(ACCEL_XOUT);                 // It sends the address of the register with which we want to communicate, starting with the ACCEL_XOUT_H (0x3B) register.
  Wire.endTransmission(false);            // It ends the transmission, but keeps the I2C channel opened (sends STOP and START).
  Wire.requestFrom(MPU_ADDR, 14);         // It configures to receive 14 bytes, starting with the chosen register (0x3B).
 
  AcX = Wire.read() << 8;                 // It reads the most significant byte. 
  AcX |= Wire.read();                     // After, it reads the least significant byte.
  AcY = Wire.read() << 8;
  AcY |= Wire.read();
  AcZ = Wire.read() << 8;
  AcZ |= Wire.read();

  //AcX = (AcX*0.06)/1000.0;  //The division by 1000 is because the result is in milligs. E.g: 16500*0.061 = 1006 milligs -> 1006/1000 = 1,006 G
  //AcY = (AcY*0.06)/1000.0;
  //AcZ = (AcZ*0.06)/1000.0;

  
  Serial.print(" |AcX = ");  Serial.print((AcX*0.06)/1000.0);
  Serial.print(" | AcY = "); Serial.print((AcY*0.06)/1000.0);
  Serial.print(" | AcZ = "); Serial.print((AcZ*0.06)/1000.0 + 0.13);
  
  delay(50);                                        
}

 
//Nunca execute nada na interrupcao, apenas setar flags!
void tCallback(void *tCall){
    _timeout = true;
}

void tCallback2(void *tCall){
    _timeout2 = true;
}
void usrInit(void){
    os_timer_setfn(&mTimer, tCallback, NULL);
    os_timer_arm(&mTimer, 10, true);

     os_timer_setfn(&mTimer2, tCallback2, NULL);
    os_timer_arm(&mTimer2, 10, true);
}

 
void setup() {
  Serial.begin(9600);
  Serial.println();
  //iniciar a interrupcao
  usrInit();

 // pinMode(LOM, INPUT); // Setup for leads off detection LO +
 // pinMode(LOm, INPUT); // Setup for leads off detection LO -

  Serial.println("nStarting MPU6050n configuration");
  initI2C();
  initMPU();
  checkMPU(MPU_ADDR);
 
  Serial.println("nConfiguration finished, starting loopn");  
 
}
 
void loop() {
  //verificacao no loop
  if (_timeout){  
    readRawMPU();    
    _timeout = false;
  }

  if (_timeout2){  
    Serial.println("Aqui");
    _timeout2 = false;
  }
  yield();  
}

/*void getECG(){
   if((digitalRead(LOM) == 1)||(digitalRead(LOm) == 1)){
      Serial.println('!');    
  
     }else{ // if both leads are connected.
      float readInt = analogRead(A0);  
      
      if (readInt > threshold && belowThreshold == true){  
          
          Serial.println(calculateBPM()); 
          belowThreshold = false;   
      
      }else if(readInt < threshold) 
        {
          belowThreshold = true;
        }
    }
}*/

/*int calculateBPM(){
  
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
}*/
