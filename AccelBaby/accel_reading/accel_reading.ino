#include <AccelBaby.h>

AccelBaby mpu(2,0);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  mpu.initI2C();
  mpu.initMPU(0,0);
  mpu.checkMPU(mpu.MPU_ADDR);
 
  Serial.println("nConfiguration finished, starting loopn"); 
}

void loop() {

  AxisMPU accel;  
  AxisMPU gyro;

  float zAngle = 0.0;

  accel = mpu.readRawACCEL();
  zAngle = mpu.getAngleAccelZ(accel.X, accel.Y, accel.Z);

  Serial.print(" Zangle: ");  Serial.println(zAngle);
  
  delay(300);
  // put your main code here, to run repeatedly:

}
