#include "capture.h"

bool bexitcapture = false;
const unsigned short IMAGEWIDTH = 640;
const unsigned short IMAGEHEIGHT = 480;

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
                //f.pos = 3*((480/2)*640+640/2);
                printf("red detected at pos = %d\n", f.pos);
                Highlight(f);
                SaveBitmapToFile( (BYTE*) f.data, f.imageWidth, f.imageHeight, 24, 0, "new_image.bmp" );
                printf("saved image\n");
            } else {
                printf("red not detected best pos = %d\n", f.pos);
printf("row=%d col=%d [%d,%d,%d]\n", f.pos/3/640, f.pos/3 - (f.pos/3/640)*640, f.data[f.pos], f.data[f.pos+1], f.data[f.pos+2]);
                Highlight(f);
for (int i = 0; i < 640*480; i++) {
  printf("pos=%d (%d,%d) [%d,%d,%d]\n", i*3, 
    i/640, i - 640*(i/640),
    f.data[3*i], f.data[3*i+1], f.data[3*i+2]);
}
                SaveBitmapToFile( (BYTE*) f.data, f.imageWidth, f.imageHeight, 24, 0, "new_image.bmp" );
                printf("saved image\n");
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
