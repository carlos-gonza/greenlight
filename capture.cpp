#include "capture.h"

bool bexitcapture = false;
const unsigned short IMAGEWIDTH = 640;
const unsigned short IMAGEHEIGHT = 480;
int best_delta = 255*3;
/**
 * capturethread
 * capture frames and  push them to the queue
 */
void capturethread(CCamera *pCamera)
{
    using std::chrono::high_resolution_clock;
    using std::chrono::milliseconds;
    Frame f;
    clock_t start, end;
    BYTE data[IMAGEWIDTH*IMAGEHEIGHT*4];

    sleep(1); // sleep a little since I noticed first grab/retrieve was black

    printf("capturetherad started\n");

    while (!bexitcapture) {
        if (fq.size() < 10) {  // keep fq size at or below 10
            start = clock();

            int res = 0;
            while ((res = pCamera->ReadFrame(0, data, sizeof(data))) == 0)
              ;

            f.captureticks = clock();

            rgbatorgb(data, sizeof(data));
            memcpy(&f.data[0], data,  sizeof(f.data));
            f.imageWidth = IMAGEWIDTH;
            f.imageHeight = IMAGEHEIGHT;

#if 1
            if (DoTestRedLight(f)) {
                printf("red, delta: %d (%d,%d) [%d,%d,%d]\n", f.delta, f.pos/3 - (f.pos/3/640)*640, 480- f.pos/3/640, f.data[f.pos+2], f.data[f.pos+1], f.data[f.pos]);
                Highlight(f);
                SaveBitmapToFile( (BYTE*) f.data, f.imageWidth, f.imageHeight, 24, 0, "new_image.bmp" );
            } else {
                if (f.delta < best_delta) {
                    best_delta = f.delta;
                    printf("no red, delta: %d (%d,%d) [%d,%d,%d]\n", f.delta, f.pos/3 - (f.pos/3/640)*640, 480- f.pos/3/640, f.data[f.pos+2], f.data[f.pos+1], f.data[f.pos]);
                    Highlight(f);
                    SaveBitmapToFile( (BYTE*) f.data, f.imageWidth, f.imageHeight, 24, 0, "new_image.bmp" );
               }
            }
#endif
            fq.push(f);

            end = clock();
        }

        // sleep enough so frame capture and push takes 100ms
        unsigned int delta_ms = (end - start)/(CLOCKS_PER_SEC/100);
        if (delta_ms < 100)
            std::this_thread::sleep_for(milliseconds(100 - delta_ms));
    }
    StopCamera();
    printf("camera released\n");
    printf("capturethread exiting\n");
}
