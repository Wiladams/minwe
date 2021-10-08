#include "gui.h"

int radius = 400;
float angle = 0;
int tx = 400;
int ty = 400;
int increment = 6;

void onFrame()
{
	// clear screen
	gAppSurface->setAllPixels(0xff000000);

	int x = tx + (radius * cos(maths::Radians(angle)));
	int y = ty + (radius * sin(maths::Radians(angle)));

	fillEllipse(*gAppSurface, x, y, 30, 30, 0xffff0000);

	refreshScreen();

	angle += increment;
}

void setup()
{
	setCanvasSize(800, 800);
	setFrameRate(30);
}