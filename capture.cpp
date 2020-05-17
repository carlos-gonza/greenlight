
#define CAPTURE
#define FRAME
#include "capture.h"


/**
 * capturethread
 * capture frames and  push them to the queue
 *
 *
 */
void capturethread(raspicam::RaspiCam *pCamera)
{
    using std::chrono::high_resolution_clock;
    using std::chrono::milliseconds;
    Frame f;
    raspicam::RaspiCam Camera = *pCamera;

    printf("Connecting to camera\n");

    if ( !Camera.open() ) {
        printf("Error opening camera\n");
        return;
    }
    cout<<"Connected to camera ="<<Camera.getId() <<" bufs="<<Camera.getImageBufferSize( )<<endl;

    unsigned char *data = new unsigned char[  Camera.getImageBufferSize( )];
    int imageWidth = Camera.getWidth();
    int imageHeight = Camera.getHeight();
    clock_t start, end;

    sleep(1); // sleep a little since I noticed first grab/retrieve was black
    int counter = 0;
    while (!bexitcapture) {

        if (fq.size() < 10) {  // keep fq size at or below 10
            start = clock();

            Camera.grab();
            Camera.retrieve ( data );
            f.captureticks = clock();

            memcpy(&f.data[0], data,  Camera.getImageBufferSize( ));
            f.imageWidth = imageWidth;
            f.imageHeight = imageHeight;
            fq.push(f);

            end = clock();
        }

        // sleep enough so frame capture and push takes 100ms
        unsigned int delta_ms = (end - start)/(CLOCKS_PER_SEC/100);
        if (delta_ms < 100)
            std::this_thread::sleep_for(milliseconds(100 - delta_ms));
    }
    Camera.release();
    printf("camera released\n");
    printf("capturethread exiting\n");
}
