#ifndef ECG_H
#define ECG_H

#include "Arduino.h"

class ECG{
public:
	ECG(int LOM, int LOm);
	int getECG();
	int calculateBPM();
	int _oldTime{0};
private:
	int _threshold{750};
	bool _belowThreshold{true};
	int _LOM;
	int _LOm;
};
#endif