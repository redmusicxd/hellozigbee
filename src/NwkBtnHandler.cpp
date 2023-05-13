#include "NwkBtnHandler.h"
#include "SwitchEndpoint.h"
#include "BlinkTask.h"

extern "C"
{
    #include "AppHardwareApi.h"
}


NwkBtnHandler::NwkBtnHandler()
{
    endpoint = NULL;
    currentState = IDLE;
    currentStateDuration = 0;
}

void NwkBtnHandler::setEndpoint(SwitchEndpoint * ep)
{
    endpoint = ep;
    ledPin = ep->ledPin;
}

const char * NwkBtnHandler::getStateName(ButtonState state)
{
    switch(state)
    {
    case IDLE: return "IDLE";
    case PRESSED1: return "PRESSED1";
    case PAUSE1: return "PAUSE1";
    case PRESSED2: return "PRESSED2";
    case PAUSE2: return "PAUSE2";
    case PRESSED3: return "PRESSED3";
    case LONG_PRESS: return "LONG_PRESS";
    }

    // Should never happen
    return "";
}

void NwkBtnHandler::handleButtonState(bool pressed, uint32 ledPin)
{
    // Let at least 20ms to stabilize button value, do not make any early decisions
    // When button state is stabilized - go through the corresponding state machine
    currentStateDuration++;
    if(currentStateDuration < 2)
        return;

    if(pressed)
    {
        // DBG_vPrintf(TRUE, "NwkBtnHandler::handleButtonState(): Button presssed! counter=%d\n", longPressCounter);
        longPressCounter++;

        if(longPressCounter > 5000/ButtonPollCycle)
        {
            changeState(LONG_PRESS);
            blinktask.init(ledPin);
            ApplicationEvent evt = {BUTTON_VERY_LONG_PRESS, 1};
            appEventQueue.send(evt);

            resetButtonStateMachine();

            longPressCounter = 0;
        }
        // blink++;
        // uint32 currentState = 0;
        // vAHI_DioSetOutput(currentState ^ (1UL << 10), currentState & (1UL << 10));
    }
    else
    {
        longPressCounter = 0;
    }
}

void NwkBtnHandler::changeState(ButtonState state)
{
    currentState = state;
    currentStateDuration = 0;

    DBG_vPrintf(TRUE, "Switching button %d state to %s\n", 1, getStateName(state));
}

void NwkBtnHandler::resetButtonStateMachine()
{
    changeState(IDLE);
}
