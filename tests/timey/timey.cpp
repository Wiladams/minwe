#include "gui.h"
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

void keyReleased(const KeyboardEvent& e) {
    if (e.keyCode == VK_ESCAPE)
        halt();
}

void setup()
{
	setCanvasSize(800, 800);
    setFrameRate(1);

	delay(2000, oldTimey);
    periodic(1000, markTime);
}