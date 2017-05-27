#define FRAME
#include "frame.h"


std::vector<RI> pri_1_left;	 // pass 1 left

void pass1(int lb, int ub, bool bisleft, int width, int height, int y, BYTE* pmatrix)
{
    RI aRI;
    bool b = true;
    int largestval = 0;
    int largestvalidx = 0;
    long bufpos = y*3*width;
    findlargestval(pmatrix, width, height, y, lb, ub, bufpos, largestval, largestvalidx);

    if (!bisleft)
        b = ((width - largestvalidx) > 10);
    if ((largestvalidx > 10) && (b)) {
        int expectedwidth =  10;
        int lval = pmatrix[bufpos + (largestvalidx - expectedwidth)*3] +
                   pmatrix[bufpos + (largestvalidx - expectedwidth)*3 +1] +
                   pmatrix[bufpos + (largestvalidx - expectedwidth)*3 + 2];
        int rval = pmatrix[bufpos + (largestvalidx + expectedwidth)*3] +
                   pmatrix[bufpos + (largestvalidx + expectedwidth)*3 +1] +
                   pmatrix[bufpos + (largestvalidx + expectedwidth)*3 + 2];
        int dleft = (largestval - lval);
        int dright = (largestval - rval);

        if ((dleft > DELTA) && (dright > DELTA) ) {
            aRI.row = height - y;

            // instead of picking brightest, pick pixel seeing brightest change
            int largestdiff = 0;
            int largestk = 0;
            for (int k = largestvalidx - expectedwidth; k <= largestvalidx ; k++) {
                int diff =
                    pmatrix[bufpos + (k+1)*3] + pmatrix[bufpos + (k+1)*3 +1] + 	pmatrix[bufpos + (k+1)*3 + 2] -
                    (pmatrix[bufpos + k*3] + pmatrix[bufpos + k*3 +1] + pmatrix[bufpos + k*3 + 2]);
                if (diff > largestdiff) {
                    largestdiff = diff;
                    largestk = k;
                }
            }
            aRI.idx = largestk;

            if (bisleft)
                pri_1_left.push_back(aRI);
            else
                pri_1_right.push_back(aRI);

            // mark first pass pixels red
            pmatrix[bufpos + largestk*3] = 0;
            pmatrix[bufpos + largestk*3 + 1] = 0;
            pmatrix[bufpos + largestk*3 + 2] = 255;
            //printf("red %s y = %d idx = %d largestdiff = %d\n", bisleft ? "left" : "right", aRI.row, aRI.idx, largestdiff);
        }
    }
}
