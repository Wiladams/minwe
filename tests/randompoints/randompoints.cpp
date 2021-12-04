#include "apphost.h"
#include "draw2d.h"

double randomRange(const float low, const float high)
{
	double frac = (double)rand() / RAND_MAX;
	double ret = low + frac * (high - low);

	return ret;
}

double random(const float rndMax)
{
	return randomRange(0, rndMax);
}


void background(PixelRGBA c)
{
	gAppSurface->setAllPixels(c);
}

void drawRandomPoints()
{

	for (int i = 0; i < 200000; i++)
	{
		int x = random(canvasWidth);
		int y = random(canvasHeight);
		int r = random(255);
		int g = random(255);
		int b = random(255);

		gAppSurface->set(x, y, { r,g,b });
	}
}

void onLoop()
{
	background(0xffdedede);

	drawRandomPoints();
	
	refreshScreen();
}

void onLoad()
{
	setCanvasSize(800, 600);
	gAppWindow->moveTo(0, 0);
	layered();
}