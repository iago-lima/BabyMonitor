#include "Arduino.h"
#include "AccelBaby.h"

AccelBaby::AccelBaby(int sda, int scl){
	_sda_pin = sda;
	_scl_pin = scl;
}

void AccelBaby::initI2C(){
	 Wire.begin(_sda_pin, _scl_pin);
}

void AccelBaby::initMPU(int gvalue, int avalue){
	setSleepOff();
	setGyroScale(gvalue);
	setAccelScale(avalue); 
}

int AccelBaby::findMPU(int mpu_addr){
	Wire.beginTransmission(mpu_addr);
  	int data = Wire.endTransmission(true);
 
  	if(data == 0){
    	Serial.print("Device found at the address: 0x");
    	Serial.println(mpu_addr, HEX);
    
  	}else{
    	Serial.println("Device wasn't found!");
  	}
  	return data;
}

void AccelBaby::checkMPU(int mpu_addr){
	if(!findMPU(MPU_ADDR)){
		int data = readRegMPU(WHO_AM_I);
		if(data == 104){
			Serial.println("MPU6050 device answered OK! (104)");
			data = readRegMPU(PWR_MGMT_1);

			if(data == 64){
				Serial.println("MPU6050 is in SLEEP mode! (64)");
			}else{
				Serial.println("MPU6050 is in ACTIVE mode!");
			}

		}else{
			Serial.println("Verify the device - MPU6050 is not available!");
		}
	}
}

void AccelBaby::writeRegMPU(int reg, int val){
	Wire.beginTransmission(MPU_ADDR);
	Wire.write(reg);
  	Wire.write(val);
  	Wire.endTransmission(true);
}

uint8_t AccelBaby::readRegMPU(uint8_t reg){
	uint8_t data;
  	Wire.beginTransmission(MPU_ADDR);
  	Wire.write(reg);
  	Wire.endTransmission(false);
  	Wire.requestFrom(MPU_ADDR, 1);
  	data = Wire.read();
  	return data;
}

void AccelBaby::setSleepOff(){
	 writeRegMPU(PWR_MGMT_1, 0);
}

void AccelBaby::setGyroScale(int value){
	writeRegMPU(GYRO_CONFIG, value);
}

void AccelBaby::setAccelScale(int value){
  writeRegMPU(ACCEL_CONFIG, value);
}

AxisMPU AccelBaby::readRawACCEL(){
	AxisMPU aux_mpu;

	Wire.beginTransmission(MPU_ADDR);
	Wire.write(ACCEL_XOUT);
	Wire.endTransmission(false);
	Wire.requestFrom(MPU_ADDR, 14);

	AcX = Wire.read() << 8;                 
	AcX |= Wire.read();                    
	AcY = Wire.read() << 8;
	AcY |= Wire.read();
	AcZ = Wire.read() << 8;
	AcZ |= Wire.read();

	aux_mpu.X = AcX;
	aux_mpu.Y = AcY;
	aux_mpu.Z = AcZ;

 	/*Serial.print(" |AcX = ");  Serial.print((AcX*0.06)/1000.0);
  	Serial.print(" | AcY = "); Serial.print((AcY*0.06)/1000.0);
  	Serial.print(" | AcZ = "); Serial.println((AcZ*0.06)/1000.0);*/
 
 	return aux_mpu;           
}

AxisMPU AccelBaby::readRawGYRO(){
	AxisMPU aux_mpu;

	Wire.beginTransmission(MPU_ADDR);
	Wire.write(GYRO_XOUT);
	Wire.endTransmission(false);
	Wire.requestFrom(MPU_ADDR, 14);

	GyX = Wire.read() << 8;                 
	GyX |= Wire.read();                    
	GyY = Wire.read() << 8;
	GyY |= Wire.read();
	GyZ = Wire.read() << 8;
	GyZ |= Wire.read();

	aux_mpu.X = GyX;
	aux_mpu.Y = GyY;
	aux_mpu.Z = GyZ;

	//DEBUG
  	/*Serial.print(" | GyX = ");  Serial.print(GyX);
  	Serial.print(" | GyY = "); Serial.print(GyY);
  	Serial.print(" | GyZ = "); Serial.println(GyZ);*/
   
   return aux_mpu;        
}

float AccelBaby::getAngleAccelZ(int16_t AcX, int16_t AcY, int16_t AcZ){
	float acelx = 0.0, acely = 0.0, acelz = 0.0;
	float AccXangle = 0.0, AccYangle = 0.0, AccZangle = 0.0;
	float const_calib = 16384.0;
	float const_gravid = 9.81;

	acelx = AcX * const_gravid / const_calib;
	acely = AcY * const_gravid / const_calib;
	acelz = AcZ * const_gravid / const_calib;

	AccXangle = (atan2(AcX, sqrt(pow(AcY,2) + pow(AcZ,2)))*180) / 3.14;
	AccYangle = (atan2(AcY, sqrt(pow(AcX,2) + pow(AcZ,2)))*180) / 3.14;
	AccZangle = (atan2(AcZ, sqrt(pow(AcX,2) + pow(AcY,2)))*180) / 3.14;

	//DEBUG
	/*Serial.print(" AngleX: "); Serial.print(AccXangle);
	Serial.print(" AngleY: "); Serial.print(AccYangle);
	Serial.print(" AngleZ: "); Serial.println(AccZangle);*/

	return AccZangle;
}