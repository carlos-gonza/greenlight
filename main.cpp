// greenlight -da -hf -w 512 -h 512
#include <iostream>
#include <ctime>
#include <cstdlib>
#include <fstream>
#include <sstream>
#include "raspicam.h"
#include <bcm2835.h>  	// control gpio
#include <thread>       // std::thread\
#include <unistd.h>
#include <mutex>

#include "capture.h"
#include "process.h"
#include "action.h"
#include "playsound.h"

using namespace std;
bool doexit = false;

raspicam::RaspiCam Camera;

std::thread t1;
std::thread t2;

//parse command line
//returns the index of a command line param in argv. If not found, return -1

int findParam ( string param,int argc,char **argv ) {
    int idx=-1;
    for ( int i=0; i<argc && idx==-1; i++ )
        if ( string ( argv[i] ) ==param ) idx=i;
    return idx;

}
//parse command line
//returns the value of a command line param. If not found, defvalue is returned
float getParamVal ( string param,int argc,char **argv,float defvalue=-1 ) {
    int idx=-1;
    for ( int i=0; i<argc && idx==-1; i++ )
        if ( string ( argv[i] ) ==param ) idx=i;
    if ( idx==-1 ) return defvalue;
    else return atof ( argv[  idx+1] );
}

raspicam::RASPICAM_EXPOSURE getExposureFromString ( string str ) {
    if ( str=="OFF" ) return raspicam::RASPICAM_EXPOSURE_OFF;
    if ( str=="AUTO" ) return raspicam::RASPICAM_EXPOSURE_AUTO;
    if ( str=="NIGHT" ) return raspicam::RASPICAM_EXPOSURE_NIGHT;
    if ( str=="NIGHTPREVIEW" ) return raspicam::RASPICAM_EXPOSURE_NIGHTPREVIEW;
    if ( str=="BACKLIGHT" ) return raspicam::RASPICAM_EXPOSURE_BACKLIGHT;
    if ( str=="SPOTLIGHT" ) return raspicam::RASPICAM_EXPOSURE_SPOTLIGHT;
    if ( str=="SPORTS" ) return raspicam::RASPICAM_EXPOSURE_SPORTS;
    if ( str=="SNOW" ) return raspicam::RASPICAM_EXPOSURE_SNOW;
    if ( str=="BEACH" ) return raspicam::RASPICAM_EXPOSURE_BEACH;
    if ( str=="VERYLONG" ) return raspicam::RASPICAM_EXPOSURE_VERYLONG;
    if ( str=="FIXEDFPS" ) return raspicam::RASPICAM_EXPOSURE_FIXEDFPS;
    if ( str=="ANTISHAKE" ) return raspicam::RASPICAM_EXPOSURE_ANTISHAKE;
    if ( str=="FIREWORKS" ) return raspicam::RASPICAM_EXPOSURE_FIREWORKS;
    return raspicam::RASPICAM_EXPOSURE_AUTO;
}

raspicam::RASPICAM_AWB getAwbFromString ( string str ) {
    if ( str=="OFF" ) return raspicam::RASPICAM_AWB_OFF;
    if ( str=="AUTO" ) return raspicam::RASPICAM_AWB_AUTO;
    if ( str=="SUNLIGHT" ) return raspicam::RASPICAM_AWB_SUNLIGHT;
    if ( str=="CLOUDY" ) return raspicam::RASPICAM_AWB_CLOUDY;
    if ( str=="SHADE" ) return raspicam::RASPICAM_AWB_SHADE;
    if ( str=="TUNGSTEN" ) return raspicam::RASPICAM_AWB_TUNGSTEN;
    if ( str=="FLUORESCENT" ) return raspicam::RASPICAM_AWB_FLUORESCENT;
    if ( str=="INCANDESCENT" ) return raspicam::RASPICAM_AWB_INCANDESCENT;
    if ( str=="FLASH" ) return raspicam::RASPICAM_AWB_FLASH;
    if ( str=="HORIZON" ) return raspicam::RASPICAM_AWB_HORIZON;
    return raspicam::RASPICAM_AWB_AUTO;
}
void processCommandLine ( int argc,char **argv,raspicam::RaspiCam &Camera ) {
    Camera.setWidth ( getParamVal ( "-w",argc,argv,1280 ) );
    Camera.setHeight ( getParamVal ( "-h",argc,argv,960 ) );
    Camera.setBrightness ( getParamVal ( "-br",argc,argv,50 ) );

    Camera.setSharpness ( getParamVal ( "-sh",argc,argv,0 ) );
    Camera.setContrast ( getParamVal ( "-co",argc,argv,0 ) );
    Camera.setSaturation ( getParamVal ( "-sa",argc,argv,0 ) );
    Camera.setShutterSpeed( getParamVal ( "-ss",argc,argv,0 ) );
    if (findParam ("-hf", argc, argv) != -1)
        Camera.setHorizontalFlip( true );
    Camera.setISO ( getParamVal ( "-iso",argc,argv,400 ) );
    if ( findParam ( "-vs",argc,argv ) !=-1 )
        Camera.setVideoStabilization ( true );
    Camera.setExposureCompensation ( getParamVal ( "-ec",argc,argv,0 ) );
    if ( findParam ( "-gr",argc,argv ) !=-1 )
        Camera.setFormat(raspicam::RASPICAM_FORMAT_GRAY);
    if ( findParam ( "-yuv",argc,argv ) !=-1 )
        Camera.setFormat(raspicam::RASPICAM_FORMAT_YUV420);
    int idx;
    if ( ( idx=findParam ( "-ex",argc,argv ) ) !=-1 )
        Camera.setExposure ( getExposureFromString ( argv[idx+1] ) );
    if ( ( idx=findParam ( "-awb",argc,argv ) ) !=-1 )
        Camera.setAWB( getAwbFromString ( argv[idx+1] ) );

}
void showUsage() {
    cout<<"Usage: "<<endl;
    cout<<"[-help shows this help]\n";
    cout<<"[-verbose sets verbose printing]\n";
    cout<<"[-test sets test mode]\n";
    cout<<"[-da dumps action images]\n";
    cout<<"[-hf flips image horizontally]\n";
    cout<<"[-gr sets gray color mode]\n";
    cout<<"[-test_speed use for test speed and no images will be saved]\n";
    cout<<"[-yuv sets yuv420 color mode]\n";
    cout<<"[-w width] [-h height] \n[-br brightness_val(0,100)]\n[-sh  sharpness_val (-100 to 100)]\n";
    cout<<"[-co contrast_val (-100 to 100)]\n[-sa saturation_val (-100 to 100)]\n";
    cout<<"[-iso ISO_val  (100 to 800)]\n[-vs turns on video stabilisation]\n[-ec exposure_compensation_value(-10,10)]\n";
    cout<<"[-ss shutter_speed (value in microsecs (max 330000)]\n[-ec exposure_compensation_value(-10,10)]\n";
    cout<<"[-exp mode (OFF,AUTO,NIGHT,NIGHTPREVIEW,BACKLIGHT,SPOTLIGHT,SPORTS,SNOW,BEACH,VERYLONG,FIXEDFPS,ANTISHAKE,FIREWORKS)]"<<endl;
    cout<<"[-awb (OFF,AUTO,SUNLIGHT,CLOUDY,TUNGSTEN,FLUORESCENT,INCANDESCENT,FLASH,HORIZON)]"<<endl;
    cout<<"[-nframes val: number of frames captured (100 default)]\n";
//      cout<<"[-ex    exposure_mode (
    cout<<endl;
}

std::mutex mtx;


#if 0
/**
 *  this thread loads a bitmap file and then saves it
 *  it also analyzes it for lines
 */
void bitmapthread(int x)
{
    printf("in bitmapthread started..\n");
    /* -- bitmap */
    int imageWidth, imageHeight;
    unsigned long size;

    BYTE* bytes = LoadBMP(&imageWidth, &imageHeight, &size, "image.bmp" );
    printf("size = %d width = %d height = %d\n", size, imageWidth, imageHeight);
    SaveBitmapToFile( (BYTE*) bytes, imageWidth, imageHeight, 24, 0, "new_image.bmp" );

    FrameStats fs;
    int firstrow = findfirstroadrow(bytes, imageWidth, imageHeight);
    bool b = GetFrameStats(bytes, imageWidth, imageHeight,  fs);
    if (fs.bleftmarker)
        printf("leftmarker: %d\n", fs.leftmarker);
    if (fs.brightmarker)
        printf("rightmarker: %d\n", fs.rightmarker);
    if (fs.bleftmarker && fs.brightmarker)
        printf("markerswidth: %d\n", fs.markerswidth);

    printf("bitmapthread returning\n");
}
test

#endif

void saveImage ( string filepath,unsigned char *data,raspicam::RaspiCam &Camera ) {
    std::ofstream outFile ( filepath.c_str(),std::ios::binary );
    if ( Camera.getFormat()==raspicam::RASPICAM_FORMAT_BGR ||  Camera.getFormat()==raspicam::RASPICAM_FORMAT_RGB) {
        outFile<<"P6\n";
    } else if ( Camera.getFormat()==raspicam::RASPICAM_FORMAT_GRAY ) {
        outFile<<"P5\n";
    } else if ( Camera.getFormat()==raspicam::RASPICAM_FORMAT_YUV420 ) { //made up format
        outFile<<"P7\n";
    }
    outFile<<Camera.getWidth() <<" "<<Camera.getHeight() <<" 255\n";
    outFile.write ( ( char* ) data,Camera.getImageBufferSize() );
}


int main ( int argc,char **argv ) {
    if ( argc==1 ) {
        cerr<<"Usage (-help for help)"<<endl;
    }

    if ( findParam ( "-help",argc,argv ) !=-1) {
        showUsage();
        return -1;
    }

    processCommandLine ( argc,argv,Camera );

    std::thread pst (playsoundthread, START);
    std::thread t1 (actionthread);
    std::thread t2 (processthread);
    std::thread t3 (capturethread, &Camera);

    printf("awaiting for threads to exit\n");

    t3.join();
    bexitaction = true;
    bexitprocess = true;
    t1.join();
    t2.join();

    printf("all threads exited\n");
}
