#ifndef ACTIONH
#define ACTIONH

#include <chrono>
#include "types.h"
#include <stdlib.h>
#include <chrono>
#include <thread>
#include <queue>

typedef enum tagANACTION {
    STOP = 0,
    START = 1,
    NOTHING = 2
} AnAction;

typedef struct tagACTION {
    long aa; 				// a set of AnAction's
    bool bplaysound; 		// play sound if true
    clock_t captureticks; 	// ticks at frame captured
    clock_t processticks; 	// ticks at frame processed
} Action;

#ifndef  ACTION
extern bool bexitaction;
extern std::queue<Action> aq;
#else
std::queue<Action> aq;
bool bexitaction;
#endif
void actionthread();
#endif
