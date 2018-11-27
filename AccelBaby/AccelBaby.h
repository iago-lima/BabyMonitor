#ifndef ACCELBABY_H
#define ACCELBABY_H

#include "Arduino.h"
#include <Wire.h> 
#include<math.h> 
#include <time.h>

#define ACCELEROMETER_SENSITIVITY 16384.0
#define GYROSCOPE_SENSITIVITY 131.0
 
#define M_PI 3.14159265359	    
 
#define dt 0.01							// 10 ms sample rate!    


class AccelBaby{

public:
AccelBaby(int sda, int scl);
void initI2C();
void initMPU(int gvalue, int avalue);
int findMPU(int mpu_addr);
void checkMPU(int mpu_addr);
void writeRegMPU(int reg, int val); 
uint8_t readRegMPU(uint8_t reg);
void setSleepOff();
void setGyroScale(int value);
void setAccelScale(int value);
float getAngleAccelZ(int16_t AcX, int16_t AcY, int16_t AcZ);
struct AxisMPU readRawACCEL();
struct AxisMPU readRawGYRO();

const int MPU_ADDR{0x68}; 
const int WHO_AM_I{0x75}; 
const int PWR_MGMT_1{0x6B}; 
const int GYRO_CONFIG{0x1B}; 
const int ACCEL_CONFIG{0x1C}; 
const int ACCEL_XOUT{0x3B};
const int GYRO_XOUT{0x43};
unsigned long pT{0};

int _sda_pin;
int _scl_pin;
int16_t AcX, AcY, AcZ, GyX, GyY, GyZ;
};

struct AxisMPU{
	int16_t X{0};
	int16_t Y{0};
	int16_t Z{0};
};

#endif