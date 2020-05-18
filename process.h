#ifndef PROCESSH
#define PROCESSH

#include <chrono>
#include <time.h>
#include <cstdio>
#include "frame.h"
#include "action.h"
#include "state.h"
#include "frame.h"

extern bool bexitprocess;
extern int CAPTURETICKS_THRESHHOLD;
void processthread();
void Highlight(Frame &f);
bool DoTestRedLight(Frame &f);
#endif
