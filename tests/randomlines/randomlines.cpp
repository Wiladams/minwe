#include "apphost.h"
#include "draw.h"
#include "maths.hpp"

using namespace maths;


void drawLines()
{
	// fill background with background color
	gAppSurface->setAllPixels(PixelRGBA(0x0));

	int strokeWeight = 1;

	for (int i = 1; i <= 10000; i++)
	{
		int x1 = random(float(canvasWidth - 1));
		int y1 = random(float(canvasHeight - 1));
		int x2 = random_int(x1-40,x1+40);
		int y2 = random_int(y1-40, y1+40);

		int r = random_int(255);
		int g = random_int(255);
		int b = random_int(255);

		draw::line_copy(*gAppSurface, x1, y1, x2, y2, PixelRGBA(r, g, b, 255), strokeWeight);
	}
}

// Every time through the application loop
// this is not timed, but occurs opportunistically
void onLoop()
{
	drawLines();

	refreshScreen();
}

// Called before anything else gets started
// Here we setup the size of the drawing canvas
void onLoad()
{
	setCanvasSize(displayWidth/2, displayHeight);

	setWindowPosition(0, 0);
	layered();
}