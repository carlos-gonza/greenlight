#ifndef FRAMEH
#define FRAMEH

#include "types.h"
#include <stdlib.h>
#include <chrono>
#include <thread>
#include <queue>
#include <vector>

#define VAL_IGNORE 180
#define DELTA 30
#define VERT 6
#define HORIZ 6
#define RANGEL 400
#define RANGEH 400

typedef struct tagFRAME {
    BYTE data[640*480*3];
    int imageWidth;
    int imageHeight;
    clock_t captureticks;
    clock_t processticks;
    unsigned long pos;
    unsigned long delta;
} Frame;

// while driving, bs=0,srl=0,btt=0,rltg=0
// while bs=0, do confirm motion, if no motion then bs=1 and break done
// if bs=1, check once for red light, if red light-> srl=1
// if bs=1 and srl=1 wait for red to green
//                 red to green: btt=1,rltg=1,srl=0 -> action,rltg=0,bs=0,btt=0
// else if bs=1 and srl=0 wait for movement
//                 movement: btt=1 -> action,bs=0,btt=0
typedef struct tagFrameStats {
    bool believestopped; // either true or false
    bool seeredlight;  // can only be true when believestopped is true
    bool believetimetogo; // true at the momentl need to go, bs=true
    bool redlighturnedgreen; // true after bs=true,srl=true
} FrameStats;

extern std::queue<Frame> fq;
#endif
