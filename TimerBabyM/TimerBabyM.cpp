#include "Arduino.h"
#include "TimerBabyM.h"


TimerBabyM::TimerBabyM(){

}

void TimerBabyM::usrInit(void (*CallSensor)(void*), void (*CallWifi)(void*)){
	os_timer_setfn(&_TimerSensor, CallSensor, NULL);
    os_timer_arm(&_TimerSensor, 1, true);

    os_timer_setfn(&_TimerWifi, CallWifi, NULL);
    os_timer_arm(&_TimerWifi, 60000, true);
}
