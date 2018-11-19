/*The original code was altered by: Marianna de Pinho Severo. This is the updated code*/

#include <Wire.h>         //Library for I2C communication
 

//Common MPU6050 address definitions
const int MPU_ADDR =      0x68; // Address of the MPU6050 sensor(0x68).
const int WHO_AM_I =      0x75; // Device identification register.
const int PWR_MGMT_1 =    0x6B; // Energy management configuration register.
const int GYRO_CONFIG =   0x1B; // Gyroscope configuration register.
const int ACCEL_CONFIG =  0x1C; // Accelerometer configuration register.
const int ACCEL_XOUT =    0x3B; // Accelerometer's X axis reading register. registro de leitura do eixo X do acelerômetro

//I2C pin definitions
const int sda_pin = D5; // Definition of I2C SDA pin.(GPIO 14 ESP8266)
const int scl_pin = D6; // Definition of I2C SCL.(GPIO 12 ESP8266)
 
//Variable to store the raw data from accelerometer. 
int16_t AcX, AcY, AcZ, Tmp, GyX, GyY, GyZ; 
 
/*********************************************
 * FUNCTION NAME: initI2C()
 * DESCRIPTION: it configures the chosen pins
 * to work as I2C pins.
 *********************************************/
void initI2C() 
{
  Wire.begin(sda_pin, scl_pin);
}

/************************************************
 * FUNCTION NAME: writeRegMPU()
 * PARAMETERS: reg is the address of the register, 
 * val is the value to be stored.
 * DESCRIPTION: it stores a value into a 
 * determined register.
 ***********************************************/ 
void writeRegMPU(int reg, int val){     //Accept a register and a value as parameters.
  
  Wire.beginTransmission(MPU_ADDR);     // It initiates the communication using the address of MPU6050.
  Wire.write(reg);                      // It sends the address of the register with which we want to communicate.
  Wire.write(val);                      // It sends the value we want to store into the register.
  Wire.endTransmission(true);           // It ends the transmission.
  
}

/**************************************************
 * FUNCTION: readRegMPU()
 * PARAMETERS: reg is the address of the register
 * we want to read.
 * DESCRIPTION: it reads the value stored in a
 * determined register
 *************************************************/
uint8_t readRegMPU(uint8_t reg){        //Accept a register as parameter.
  
  uint8_t data;                         //Variable used to store the value read.
  Wire.beginTransmission(MPU_ADDR);     // It initiates the communication using the address of MPU6050.
  Wire.write(reg);                      // It sends the address of the register with which we want to communicate.
  Wire.endTransmission(false);          // It ends the transmission, but keeps the I2C channel opened (sends STOP and START).
  Wire.requestFrom(MPU_ADDR, 1);        // It configures to receive 1 byte from the chosen register.
  data = Wire.read();                   // It reads the byte and stores into the data variable.
  return data;                          // It returns the data stored in data.
}

 /***************************************************
  * FUNCTION findMPU()
  * PARAMETERS: mpu_addr is the address of the device
  * we are searching for.
  * DESCRIPTION: it searches the device with the
  * address specified by the parameter.
  ***************************************************/
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

/*******************************************************
 * FUNCTION: checkMPU()
 * PARAMETERS: mpu_addr is the address of the device we
 * want to communicate with.
 * DESCRIPTION: it verifies if the device is active and
 * responds.
 ******************************************************/ 
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

/********************************************************
 * FUNCTION: initMPU()
 * DESCRIPTION: it initiates de MPU6050 sensor.
 ********************************************************/
void initMPU(){
  setSleepOff();   //It activates the sensor
  setGyroScale();  //It configures the gyroscope scale.
  setAccelScale(); //It configures the accelerometer scale.
  
}

/********************************************************
 * FUNCTION: setSleepOff()
 * DESCRIPTION: it configures the sleep bit, turning active
 * the device.
 *******************************************************/
void setSleepOff(){
  
  writeRegMPU(PWR_MGMT_1, 0); // It writes zero into the energy management register.
  
}

/**************************************************
 * FUNCTION: setGyroScale()
 * DESCRIPTION: it configures the gyro scale.
 * Gyro scale register: 0x1B[4:3] 0 is 250°/s    
 * FS_SEL  Full Scale Range
 *    0        ± 250 °/s      0b00000000
 *    1        ± 500 °/s      0b00001000
 *    2        ± 1000 °/s     0b00010000
 *    3        ± 2000 °/s     0b00011000
 *************************************************/
void setGyroScale(){
  
  writeRegMPU(GYRO_CONFIG, 0);
  
}

/***************************************************
 * FUNCTION: setAccelScale()
 * DESCRIPTION: it configures the accel scale.
 * Accel scale register: 0x1C[4:3] 0 is 250°/s
 * AFS_SEL   Full Scale Range
 *    0           ± 2g            0b00000000
 *    1           ± 4g            0b00001000
 *    2           ± 8g            0b00010000
 *    3           ± 16g           0b00011000
 **************************************************/
void setAccelScale(){
  
  writeRegMPU(ACCEL_CONFIG, 0);

}

/***************************************************
 * FUNCTION: readRawMPU()
 * DESCRIPTION: it reads the raw data from the sensor.
 * In totally, there are 14 bytes, being 4 bytes for
 * each axis and 2 bytes for temperature: 
 * 0x3B 59 ACCEL_XOUT[15:8]
 * 0x3C 60 ACCEL_XOUT[7:0]
 * 0x3D 61 ACCEL_YOUT[15:8]
 * 0x3E 62 ACCEL_YOUT[7:0]
 * 0x3F 63 ACCEL_ZOUT[15:8]
 * 0x40 64 ACCEL_ZOUT[7:0]
 * 
 * 0x41 65 TEMP_OUT[15:8]
 * 0x42 66 TEMP_OUT[7:0]
 * 
 * 0x43 67 GYRO_XOUT[15:8] 
 * 0x44 68 GYRO_XOUT[7:0]
 * 0x45 69 GYRO_YOUT[15:8]
 * 0x46 70 GYRO_YOUT[7:0]
 * 0x47 71 GYRO_ZOUT[15:8]
 * 0x48 72 GYRO_ZOUT[7:0] 
 ***************************************************/
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
 
  Tmp = Wire.read() << 8;
  Tmp |= Wire.read();
 
  GyX = Wire.read() << 8;
  GyX |= Wire.read();
  GyY = Wire.read() << 8;
  GyY |= Wire.read();
  GyZ = Wire.read() << 8;
  GyZ |= Wire.read(); 

  //AcX = (AcX*0.06)/1000.0;  //The division by 1000 is because the result is in milligs. E.g: 16500*0.061 = 1006 milligs -> 1006/1000 = 1,006 G
  //AcY = (AcY*0.06)/1000.0;
  //AcZ = (AcZ*0.06)/1000.0;
  //Tmp = Tmp/340.00+36.53;
  
  Serial.print(" |AcX = ");  Serial.print((AcX*0.06)/1000.0);
  Serial.print(" | AcY = "); Serial.print((AcY*0.06)/1000.0);
  Serial.print(" | AcZ = "); Serial.print((AcZ*0.06)/1000.0);
  Serial.print(" | Tmp = "); Serial.print(Tmp/340.00+36.53);
  Serial.print(" | GyX = "); Serial.print(GyX);
  Serial.print(" | GyY = "); Serial.print(GyY);
  Serial.print(" | GyZ = "); Serial.println(GyZ);
 
  delay(50);                                        
}

void setup() {
  
  Serial.begin(9600);
 
  Serial.println("nStarting MPU6050n configuration");
  initI2C();
  initMPU();
  checkMPU(MPU_ADDR);
 
  Serial.println("nConfiguration finished, starting loopn");  
  
}
 
void loop() {
  
  readRawMPU();    // It reads sensor data.
  delay(100); 
   
}
