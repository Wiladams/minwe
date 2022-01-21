#include "apphost.h"
#include "draw2d.h"

constexpr int MAXPOINTS = 200000;

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

	for (int i = 0; i < MAXPOINTS; i++)
	{
		int x = random_int(canvasWidth);
		int y = random_int(canvasHeight);
		uint32_t r = random_int(255);
		uint32_t g = random_int(255);
		uint32_t b = random_int(255);

		gAppSurface->set(x, y, { r,g,b });
	}
}

void onLoop()
{
	background(PixelRGBA(0x0));

	drawRandomPoints();
	
	refreshScreen();
}

void onLoad()
{
	setCanvasSize(800, 600);
	gAppWindow->moveTo(0, 0);
	layered();
}