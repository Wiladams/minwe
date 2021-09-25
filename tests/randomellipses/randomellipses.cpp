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

inline PixelRGBA randomColor()
{
	return { (int)random(255), (int)random(255), (int)random(255) };
}


void background(PixelRGBA c)
{
	gAppSurface->setAllPixels(c);
}

void drawRandomEllipses(PixelRect bounds)
{
	for (int i = 0; i < 1000; i++)
	{
		int cx = randomRange(bounds.x+4, bounds.x+bounds.width-1-4);
		int cy = randomRange(bounds.y + 4, bounds.y + bounds.height - 1 - 4);

		int xRadius = randomRange(4, 60);
		int yRadius = randomRange(4, 60);
		fillEllipse(*gAppSurface, cx, cy, xRadius, yRadius, randomColor());
	}
	

}

void onLoop()
{
	background(0xffcccccc);
	drawRandomEllipses({ 0,0,canvasWidth,canvasHeight });
	refreshScreen();
}

void onLoad()
{
	setCanvasSize(1024, 768);
}