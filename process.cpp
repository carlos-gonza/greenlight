//#define  PROCESS
#include "process.h"
#include "bmp.h"
#include <math.h>

Frame gprevFrame;	// previously captured frame
TheState theState;
std::queue<Frame> fq;

#define VAL(POS) pmatrix[(POS)] + pmatrix[(POS)+1] + pmatrix[(POS)+2]
#define VALPREV(POS) pprevmatrix[(POS)] + pmatrix[(POS)+1] + pmatrix[(POS)+2]

#define DELTAVAL(POS) abs(VAL(POS) - VALPREV(POS))
/**
 * RGBCloseToColor
 * return true if RGB is close to a Color
 */
bool RGBCloseToColor(int R, int G, int B, int ColorR, int ColorG, int ColorB,
  int &delta)
{
    const int CLOSECOLORTHRESHHOLD = 35;
#if 1
    delta = abs(R - ColorR) + abs(G - ColorG) + abs(B - ColorB);
#else
    delta = (int) sqrt(abs(R - ColorR)*abs(R-ColorR) + abs(G - ColorG)*abs(G - ColorG) + abs(B - ColorB)*abs(B - ColorB));
#endif
    return ((abs(R - ColorR) < CLOSECOLORTHRESHHOLD) &&
            (abs(G - ColorG) < CLOSECOLORTHRESHHOLD) &&
            (abs(B - ColorB) < CLOSECOLORTHRESHHOLD));
}

/**
 *  DoTestMoving
 *  return true if between gprevFrame and f, we appear to
 *  be moving.
 */
bool DoTestMoving(Frame &f)
{
    const unsigned long MOVINGTHRESHHOLD = 100;
    int w = f.imageWidth;
    int h = f.imageHeight;
    BYTE *pmatrix = (BYTE *) &f.data[0];
    BYTE *pprevmatrix = (BYTE *) &gprevFrame.data[0];
    unsigned long sumval = 0;

    for (int rowpos = 0; rowpos < h; rowpos++) {
        for (int xpos = 0; xpos < w; xpos++) {
            sumval += DELTAVAL(3*(rowpos*w + xpos));
        }
    }

    return sumval > MOVINGTHRESHHOLD;
}

bool DoTestStopped(Frame &f)
{
    const unsigned long STOPPEDTHRESHHOLD = 15000000;
    int w = f.imageWidth;
    int h = f.imageHeight;
    BYTE *pmatrix = (BYTE *) &f.data[0];
    BYTE *pprevmatrix = (BYTE *) &gprevFrame.data[0];
    unsigned long sumval = 0;

    for (int rowpos = 0; rowpos < h; rowpos++) {
        for (int xpos = 0; xpos < w; xpos++) {
            sumval += DELTAVAL(3*(rowpos*w + xpos));
        }
    }

    return sumval < STOPPEDTHRESHHOLD;
}

/**
 * DoTestRedLight
 * return true if we see a red light signal
 * only use frame f
 */
bool DoTestRedToGreen(Frame &f)
{
    int pos = 0;
    BYTE *pmatrix = (BYTE *) &f.data[0];
    pos = theState.getLightPos();
    f.pos = pos;
    int B = pmatrix[pos];
    int G = pmatrix[pos+1];
    int R = pmatrix[pos+2];
    int delta = 0;
    return (RGBCloseToColor(R, G, B, 0, 255, 0, delta));
}

// highli ght a yellow box at pos
void Highlight(Frame &f)
{
    int w = f.imageWidth;
    int h = f.imageHeight;
    BYTE *pmatrix = (BYTE *) &f.data[0];
    int rowpos_ = f.pos/3/w;
    int rowpos_lb, rowpos_ub, xpos_lb, xpos_ub;
    int xpos_ = f.pos/3 - rowpos_*w;
    if (rowpos_ > 5)
        rowpos_lb = rowpos_ - 5;
    else
        rowpos_lb = 0;
    if (rowpos_ + 5 > h)
        rowpos_ub = h;
    else
        rowpos_ub = rowpos_ + 5;

    if (xpos_ > 5)
        xpos_lb = xpos_ - 5;
    else
        xpos_lb = 0;
    if (xpos_ + 5 > w)
        xpos_ub = w;
    else
        xpos_ub = xpos_ + 5;

    for (int rowpos = rowpos_lb; rowpos < rowpos_ub; rowpos++) {
        int pos = 3*(rowpos*w + xpos_lb);
        // mark yellow
        f.data[pos] = 0;
        f.data[pos+1] = 255;
        f.data[pos+2] = 255;
        pos = 3*(rowpos*w + xpos_ub);
        // mark yellow
        f.data[pos] = 0;
        f.data[pos+1] = 255;
        f.data[pos+2] = 255;
    }
    for (int xpos = xpos_lb; xpos < xpos_ub; xpos++) {
        int pos = 3*(rowpos_lb*w + xpos);
        // mark yellow
        f.data[pos] = 0;
        f.data[pos+1] = 255;
        f.data[pos+2] = 255;
        pos = 3*(rowpos_ub*w + xpos);
        // mark yellow
        f.data[pos] = 0;
        f.data[pos+1] = 255;
        f.data[pos+2] = 255;
    }
}

bool DoTestRedLight(Frame &f)
{
    int w = f.imageWidth;
    int h = f.imageHeight;
    BYTE *pmatrix = (BYTE *) &f.data[0];
    int delta = 0;
    int mindelta = 255*3;

    for (int pos = 0; pos < w*h; pos++) {
            int B = pmatrix[3*pos];
            int G = pmatrix[3*pos+1];
            int R = pmatrix[3*pos+2];
            if (RGBCloseToColor(R, G, B, 255, 0, 0, delta)) {
                f.pos = 3*pos;
                return true;
            }
            // update with closest match
            if (delta < mindelta) {
                f.pos = 3*pos;
                f.delta = delta;
                mindelta = delta;
            }
    }

    return false;
}

bool DoTestLocked(Frame &f)
{
    return true;
}

bool DoTestLockMoving(Frame &f)
{
    return true;
}

bool DoTestGo(Frame &f)
{
    return true;
}

/**
* Given Frames f and gprevFrame, determine if the delta
* agrees with testState.
*
* For example, if testState is STOPPED, then we return true
* if delta shows hardly a difference.
* If testState is REDTOGREEN, we return true only if prev showed
* red light and f now shows green instead.
* We use global theState for additional information needed, like
* coordinates of where the signal light is.
*/
bool DoTest(Frame &f, CurrentState testState)
{
    switch (testState) {
    case MOVING:
        return DoTestMoving(f);
        break;
    case STOPPED:
        return DoTestStopped(f);
        break;
    case REDLIGHT:
        return DoTestRedLight(f);
        break;
    case REDTOGREEN:
        return DoTestRedToGreen(f);
        break;
    case LOCKED:
        return DoTestLocked(f);
        break;
    case LOCKMOVING:
        return DoTestLockMoving(f);
        break;
    case GO:
        return DoTestGo(f);
        break;
    default:
        break;
    }
    return true;
}

/**
* bool processFrame(Frame &f, Action &a)
* process frame f and return if it dictates
* action necessary, in which case action
* is returned by parameter Action &a
*/
bool processFrame(Frame &f, Action &a)
{
    State s;
    bool bActionNecessary = false;

    CurrentState testState = theState.TestFor();
    if (true == DoTest(f, testState)) {
        theState.setCurrentState(testState);
        theState.setLightPos(f.pos);
        if ((REDTOGREEN == testState) ||
                (LOCKMOVING == testState))  {
            a.aa = START;
            a.captureticks = f.captureticks;
            bActionNecessary = true;
        }
    }

    gprevFrame = f;
    return bActionNecessary;
}

/**
 * processqueue
 * pop from the frameque fq and process recent frames only
 * processed frames requiring an action are pushed to the action queue aq
 *
 *
 */
void processthread()
{
    using std::chrono::high_resolution_clock;
    using std::chrono::milliseconds;
    printf("processthread enterd\n");

    while (!bexitprocess) {
        if (!fq.empty()) {
            Frame f = fq.front();
            fq.pop();
            if (aq.size() < 10) { // keep aq at or below 10
                clock_t now = clock();
                if ((now - f.captureticks) < CAPTURETICKS_THRESHHOLD) {
                    // frame was captured recently so process it
                    Action a;
                    bool bTakeAction = processFrame(f, a);
                    if (bTakeAction) {
                        aq.push(a);
                    }
                }
            }
        } else
            std::this_thread::sleep_for(milliseconds(10));
    }
    printf("processthread exiting\n");
}
