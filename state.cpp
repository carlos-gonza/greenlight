#include "state.h"

// constructor
TheState::TheState()
{
    cs = MOVING;
    lightPos = 0;
}

void TheState::setCurrentState(CurrentState cs)
{
    this->cs = cs;
}

CurrentState TheState::getCurrentState() {
    return this->cs;
}

void TheState::setLightPos(int pos) {
    lightPos = pos;
}

int TheState::getLightPos() {
    return lightPos;
}

/**
 * TestFor
 * Returns what state to test for according to
 * the current state cs.
 * For example if cs is MOVING, then we state for STOPPED
 */
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
