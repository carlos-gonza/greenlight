
#ifndef CAPTUREH
#define CAPTUREH
#include <chrono>
#include <queue>
#include "camera.h"
#include "bmp.h"
#include "process.h"
#include <thread>
#include "frame.h"
#include <unistd.h>

using namespace std;

extern bool bexitcapture;
void capturethread(CCamera *pCamera);
#endif
