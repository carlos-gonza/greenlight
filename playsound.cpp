#define PLAYSOUND
#include "action.h"
#include <iostream>
#include <cstdlib>

using namespace std;

void playsoundthread(AnAction aa)
{
    cout << "in playsoundthread " <<(long) aa <<endl;

    switch (aa) {
    case STOP:
        cout << "stop" << endl;
        system("mpg321 -q -g 50 stop.mp3");
        break;
    case START:
        cout << "start" << endl;
        system("mpg321 -q -g 50 start.mp3");
        break;
    default:
        cout << "default" << endl;
        break;

    }
}
