// greenlight -da -hf -w 512 -h 512
#include <ctime>
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <bcm2835.h>  	// control gpio
#include <thread> 
#include <mutex>

#include "capture.h"
#include "process.h"
#include "action.h"
#include "playsound.h"

#include "camera.h"
#include "graphics.h"

using namespace std;
int CAPTURETICKS_THRESHHOLD = CLOCKS_PER_SEC/10; // tenth of a second

bool doexit = false;
bool bexitprocess = false;

int main ( int argc,char **argv ) {
    CCamera* cam = StartCamera(640, 480,30,1,true);

    std::thread pst (playsoundthread, START);
    std::thread t1 (actionthread);
    std::thread t2 (processthread);
    std::thread t3 (capturethread, cam);

    printf("awaiting for threads to exit\n");

    t3.join();
    bexitaction = true;
    bexitprocess = true;
    t1.join();
    t2.join();

    printf("all threads exited\n");
}
