#include "state.h"
#include <stdio.h>

// constructor
TheState::TheState()
{
    setCurrentState(MOVING);
    lightPos = 0;
}

void TheState::setCurrentState(CurrentState cs)
{
    switch (cs) {
    case MOVING:
        printf("state is MOVING\n"); 
        break;
    case LOCKMOVING:
        printf("state is LOCKMOVING\n"); 
        break;
    case STOPPED:
        printf("state is STOPPED\n"); 
        break;
    case REDLIGHT:
        printf("state is REDLIGHT\n"); 
        break;
    case LOCKED:
        printf("state is LOCKED\n"); 
        break;
    case GO:
        printf("state is GO\n"); 
        break;
    default:
        printf("state is unknown\n"); 
        break;
    }
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
    case REDTOGREEN:
        return GO;
        break;
    case GO:
        return MOVING;
        break;
    case LOCKED:
        return LOCKMOVING;
        break;
    case LOCKMOVING:
        return GO;
    default:
        return STOPPED;
        break;
    }
}
