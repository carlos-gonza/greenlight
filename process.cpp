#define  PROCESS
#include "process.h"
// carlos
Frame gprevFrame;	// previously captured frame
TheState theState;

#define VAL(POS) pmatrix[(POS)] + pmatrix[(POS)+1] + pmatrix[(POS)+2]
#define VALPREV(POS) pprevmatrix[(POS)] + pmatrix[(POS)+1] + pmatrix[(POS)+2]

#define DELTAVAL(POS) abs(VAL(POS) - VALPREV(POS))
/**
 * RGBCloseToColor
 * return true if RGB is close to a Color
 */
bool RGBCloseToColor(int R, int G, int B, int ColorR, int ColorG, int ColorB)
{
    const int CLOSECOLORTHRESHHOLD = 10;
    return ((abs(R - ColorR) < CLOSECOLORTHRESHHOLD) &&
            (abs(G - ColorG) < CLOSECOLORTHRESHHOLD) &&
            (abs(B - ColorB) < CLOSECOLORTHRESHHOLD));
}

/**
 *  DoTestMoving
 *  return true if between gprevFrame and f, we appear to
 *  be moving.
 */
bool DoTestMoving(Frame f)
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

bool DoTestStopped(Frame f)
{
    const unsigned long STOPPEDTHRESHHOLD = 100;
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
bool DoTestRedToGreen(Frame f)
{
    int pos = 0;
    BYTE *pmatrix = (BYTE *) &f.data[0];
    pos = theState.getLightPos();
    int R = pmatrix[pos];
    int G = pmatrix[pos+1];
    int B = pmatrix[pos+2];
    return (RGBCloseToColor(R, G, B, 0, 255, 0));
}

bool DoTestRedLight(Frame f)
{
    int w = f.imageWidth;
    int h = f.imageHeight;
    BYTE *pmatrix = (BYTE *) &f.data[0];

    for (int rowpos = 0; rowpos < h; rowpos++) {
        for (int xpos = 0; xpos < w; xpos++) {
            int pos = 3*(rowpos*w + xpos);
            int R = pmatrix[pos];
            int G = pmatrix[pos+1];
            int B = pmatrix[pos+2];
            if (RGBCloseToColor(R, G, B, 255, 0, 0)) {
                theState.setLightPos(pos);
                return true;
            }
        }
    }

    return false;
}

bool DoTestLocked(Frame f)
{
    return true;
}

bool DoTestLockMoving(Frame f)
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
bool DoTest(Frame f, CurrentState testState)
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

    // analyze f.Grid
    BYTE *pmatrix = (BYTE *) &f.data[0];

    // pmatrix, f.imageWidth, f.imageHeight
    CurrentState testState = theState.TestFor();
    if (true == DoTest(f, testState)) {
        theState.setCurrentState(testState);
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

            clock_t now = clock();
            if ((now - f.captureticks) < CAPTURETICKS_THRESHHOLD) {
                // frame was captured recently so process it
                Action a;
                bool bTakeAction = processFrame(f, a);
                if (bTakeAction) {
                    aq.push(a);
                }
            }
        } else
            std::this_thread::sleep_for(milliseconds(10));
    }
    printf("processthread exiting\n");
}
