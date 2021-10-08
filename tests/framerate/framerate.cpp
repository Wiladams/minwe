
#include "gui.h"

void onFrame()
{
	printf("Frame draw: %zd\n", frameCount);
}
void setup()
{
	setCanvasSize(800, 800);
	setFrameRate(1);
}