#include "ButtonHandler.h"

extern "C"
{
    #include "AppHardwareApi.h"
}


ButtonHandler::ButtonHandler()
{
    endpointId = 0xff; //Unknown ID

    currentState = IDLE;
    currentStateDuration = 0;

    switchType = SWITCH_TYPE_TOGGLE;
    switchMode = SWITCH_MODE_FRONT;
    maxPause = 30;
    longPressDuration = 100;
}

void ButtonHandler::setEndpointId(uint8 id)
{
    endpointId = id;
}

inline void ButtonHandler::sendButtonEvent(ApplicationEventType evtType)
{
    ApplicationEvent evt = {evtType, endpointId};
    appEventQueue.send(evt);
}

const char * ButtonHandler::getStateName(ButtonState state)
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

void ButtonHandler::setSwitchType(SwitchType type)
{
    switchType = type;
    changeState(IDLE);
}

void ButtonHandler::setLocalSwitchMode(LocalSwitchMode mode)
{
    switchMode = mode;
    changeState(IDLE);
}

void ButtonHandler::setMaxPause(uint16 value)
{
    maxPause = value/ButtonPollCycle;
    changeState(IDLE);
}

void ButtonHandler::setMinLongPress(uint16 value)
{
    longPressDuration = value/ButtonPollCycle;
    changeState(IDLE);
}

void ButtonHandler::changeState(ButtonState state)
{
    currentState = state;
    currentStateDuration = 0;

    DBG_vPrintf(TRUE, "Switching button %d state to %s\n", endpointId, getStateName(state));
}

void ButtonHandler::buttonStateMachineToggle(bool pressed)
{
    // The state machine
    switch(currentState)
    {
        case IDLE:
            if(pressed)
            {
                changeState(PRESSED1);
                sendButtonEvent(BUTTON_ACTION_SINGLE);

                if(switchMode == SWITCH_MODE_FRONT)
                    sendButtonEvent(SWITCH_TRIGGER);
            }
            break;

        case PRESSED1:
            if(!pressed)
                changeState(IDLE);

            break;

        default:
            changeState(IDLE);  // How did we get here?
            break;
    }
}

void ButtonHandler::buttonStateMachineMomentary(bool pressed)
{
    // The state machine
    switch(currentState)
    {
        case IDLE:
            if(pressed)
            {
                changeState(PRESSED1);
                sendButtonEvent(BUTTON_PRESSED);

                if(switchMode == SWITCH_MODE_FRONT)
                    sendButtonEvent(SWITCH_ON);
            }
            break;

        case PRESSED1:
            if(!pressed)
            {
                changeState(IDLE);
                sendButtonEvent(BUTTON_RELEASED);

                if(switchMode == SWITCH_MODE_FRONT)
                    sendButtonEvent(SWITCH_OFF);
            }

            break;

        default:
            changeState(IDLE); // How did we get here?
            break;
    }
}

void ButtonHandler::buttonStateMachineMultifunction(bool pressed)
{
    // The state machine
    switch(currentState)
    {
        case IDLE:
            if(pressed)
            {
                changeState(PRESSED1);

                if(switchMode == SWITCH_MODE_FRONT)
                    sendButtonEvent(SWITCH_TRIGGER);
            }
            break;

        case PRESSED1:
            if(pressed && currentStateDuration > longPressDuration)
            {
                changeState(LONG_PRESS);
                sendButtonEvent(BUTTON_PRESSED);

                if(switchMode == SWITCH_MODE_LONG)
                    sendButtonEvent(SWITCH_TRIGGER);
            }

            if(!pressed)
            {
                changeState(PAUSE1);
            }

            break;

        case PAUSE1:
            if(!pressed && currentStateDuration > maxPause)
            {
                changeState(IDLE);
                sendButtonEvent(BUTTON_ACTION_SINGLE);

                if(switchMode == SWITCH_MODE_SINGLE)
                    sendButtonEvent(SWITCH_TRIGGER);
            }

            if(pressed)
                changeState(PRESSED2);

            break;

        case PRESSED2:
            if(!pressed)
            {
                changeState(PAUSE2);
            }

            break;

        case PAUSE2:
            if(!pressed && currentStateDuration > maxPause)
            {
                changeState(IDLE);
                sendButtonEvent(BUTTON_ACTION_DOUBLE);

                if(switchMode == SWITCH_MODE_DOUBLE)
                    sendButtonEvent(SWITCH_TRIGGER);
            }

            if(pressed)
            {
                changeState(PRESSED3);
            }

            break;

        case PRESSED3:
            if(!pressed)
            {
                changeState(IDLE);

                if(switchMode == SWITCH_MODE_TRIPPLE)
                    sendButtonEvent(SWITCH_TRIGGER);

                sendButtonEvent(BUTTON_ACTION_TRIPPLE);
            }

            break;

        case LONG_PRESS:
            if(!pressed)
            {
                changeState(IDLE);

                sendButtonEvent(BUTTON_RELEASED);
            }

            break;

        default: break;
    }
}

void ButtonHandler::handleButtonState(bool pressed)
{
    // Let at least 20ms to stabilize button value, do not make any early decisions
    // When button state is stabilized - go through the corresponding state machine
    currentStateDuration++;
    if(currentStateDuration < 2)
        return;

    switch(switchType)
    {
    case SWITCH_TYPE_TOGGLE:
        buttonStateMachineToggle(pressed);
        break;
    case SWITCH_TYPE_MOMENTARY:
        buttonStateMachineMomentary(pressed);
        break;
    case SWITCH_TYPE_MULTIFUNCTION:
        buttonStateMachineMultifunction(pressed);
        break;
    default:
        break;
    }
}

void ButtonHandler::resetButtonStateMachine()
{
    changeState(IDLE);
}
