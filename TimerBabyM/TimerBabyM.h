#ifndef TIMERBABYM_H
#define TIMERBABYM_H

#include "Arduino.h"
extern "C"{
#include "user_interface.h"
}

class TimerBabyM{
public:
	TimerBabyM();
	void  usrInit(void (*CallSensor)(void*),void (*CallWifi)(void*));
	os_timer_t _TimerSensor;
	os_timer_t _TimerWifi;
	bool _timeoutSensor{false};
	bool _timeoutWifi{false};
};

#endif