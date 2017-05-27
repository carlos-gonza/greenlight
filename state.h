#ifndef STATEH
#define STATEH

typedef struct STATE_TAG {
    bool bstopped;
} State;

typedef enum CURRENTSTATE {
    MOVING = 0,
    STOPPED = 1,
    REDLIGHT = 2,
    REDTOGREEN = 3,
    LOCKED = 4,
    LOCKMOVING = 5,
    GO = 6
} CurrentState;

/* MOVING -> STOPPED -> REDLIGHT -> REDTOGREEN -> GO -> MOVING
               +-->     LOCKED -> LOCKMOVING   -->+
*/

class TheState {
private:
    CurrentState cs;
    int lightXCoord, lightYCoord;

public:
    TheState();
    void setCurrentState(CurrentState cs);
    CurrentState getCurrentState();
    CurrentState TestFor();
    void setLightCoordinates(int x, int y);
    bool getLightCoordinates(int &x, int &y);
};

#endif
