/*
    Simple application to test out timing
    periodic
    delay

    Simply close the window to stop the app

    Must be run as a windows/console
*/
#include "apphost.h"
#include "timing.h"


using namespace timing;
StopWatch sw;

void oldTimey()
{
    printf("old timey\n");
}

void markTime()
{
    printf("Mark Time: %d\n", (int)sw.seconds());
}

void onLoad()
{
	setCanvasSize(800, 800);
    setTitle("timey");

	delay(2000, oldTimey);
    periodic(1000, markTime);
}