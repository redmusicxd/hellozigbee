#ifndef NWKBUTTONHANDLER_H
#define NWKBUTTONHANDLER_H

#include "ButtonModes.h"
#include "IButtonHandler.h"
#include "AppQueue.h"
#include "BlinkTask.h"

#include <jendefs.h>

class SwitchEndpoint;


class NwkBtnHandler: public IButtonHandler
{
    SwitchEndpoint * endpoint;

    BlinkTask blinktask;

    uint32 ledPin;

    uint32 currentStateDuration;

    uint32 longPressCounter;

    enum ButtonState
    {
        IDLE,
        PRESSED1,
        PAUSE1,
        PRESSED2,
        PAUSE2,
        PRESSED3,
        LONG_PRESS
    };

    ButtonState currentState;

public:
    NwkBtnHandler();

    void setEndpoint(SwitchEndpoint * ep);

protected:
    virtual void handleButtonState(bool pressed, uint32 ledPin);
    virtual void resetButtonStateMachine();

    virtual void changeState(ButtonState state);

    const char * getStateName(ButtonState state);
};

#endif // NWKBTNHANDLER_H
