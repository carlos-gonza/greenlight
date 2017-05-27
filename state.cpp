#include "state.h"

// constructor
TheState::TheState()
{
    cs = MOVING;
    lightXCoord = lightYCoord = 0;
}

void TheState::setCurrentState(CurrentState cs)
{
    this->cs = cs;
}

CurrentState TheState::getCurrentState() {
    return this->cs;
}

void TheState::setLightCoordinates(int x, int y) {
    lightXCoord = x;
    lightYCoord = y;
}

bool TheState::getLightCoordinates(int &x, int &y) {
    x = lightXCoord;
    y = lightYCoord;
}

CurrentState  TheState::TestFor()
{
    switch (cs) {
    case MOVING:
        return STOPPED;
        break;
    case STOPPED:
        return REDLIGHT;
        break;
    case REDLIGHT:
        return REDTOGREEN;
        break;
    case LOCKED:
        return LOCKMOVING;
        break;
    default:
        return STOPPED;
        break;
    }
}
