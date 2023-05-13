#include "BlinkTask.h"
#include "ZigbeeDevice.h"

extern "C"
{
#include "AppHardwareApi.h"
#include "dbg.h"
}


static const uint32 FAST_BLINK_PERIOD = ZTIMER_TIME_MSEC(200);
static const uint32 SLOW_BLINK_PERIOD = ZTIMER_TIME_MSEC(1000);

// Note: Object constructors are not executed by CRT if creating a global var of this object :(
// So has to be created explicitely in vAppMain() otherwise VTABLE will not be initialized properly
BlinkTask::BlinkTask()
{
}

void BlinkTask::init(uint8 pin)
{
    ledPin.init(pin);

    PeriodicTask::init();
    startTimer(200);
}

void BlinkTask::stop(){
    stopTimer();
}

void BlinkTask::timerCallback()
{
    // toggle LED
    ledPin.toggle();

    if(ZigbeeDevice::getInstance()->isJoined()){
        stopTimer();
        ledPin.off();
    }
    if(ZigbeeDevice::getInstance()->isLeft()){
        stopTimer();
        ledPin.off();
    }

    //Restart the timer
    startTimer(FAST_BLINK_PERIOD);
}
