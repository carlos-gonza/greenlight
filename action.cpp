#define ACTION
#include "action.h"
#include "process.h"
#include "playsound.h"
#include <thread>

/**
 * doaction
 *
 */
void doaction(Action a)
{
    if (a.aa & START) {
        if (a.bplaysound) {
            std::thread pst (playsoundthread, START);
            pst.detach();
        }
    }
}

/**
 * actionthread
 * pop from the action queue aq and take action on recent elements only
 */
void actionthread()
{
    using std::chrono::high_resolution_clock;
    using std::chrono::milliseconds;
    Action a;

    printf("actionthread entered\n");

    while(!bexitaction) {
        if (!aq.empty()) {
            a = aq.front();
            aq.pop();
            clock_t now = clock();
            if ((now - a.captureticks) < 10*CAPTURETICKS_THRESHHOLD) {
                doaction(a);
            }
        } else
            std::this_thread::sleep_for(milliseconds(10));
    }
    printf("actionthread exiting\n");
}
