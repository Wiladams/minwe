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

PixelRGBA randomColor()
{
	return { (int)random(255),(int)random(255), (int)random(255) };
}

void background(PixelRGBA c)
{
	gAppSurface->setAllPixels(c);
}


void drawRandomTriangles(PixelRect bounds)
{
	//fillTriangle(*gAppSurface, 400, 50, 200, 200, 600, 400, 0xffff0000, bounds);
	//*
	for (int i = 1; i <= 1000; i++)
	{
		int x1 = (int)random(canvasWidth);
		int y1 = (int)random(canvasHeight);
		int x2 = (int)randomRange(x1 - 120, x1 + 120);
		int y2 = (int)randomRange(y1+4, y1 + 120);
		int x3 = (int)randomRange(x2 - 120, x2 + 120);
		int y3 = (int)randomRange(y2+4, y2 + 120);

		fillTriangle(*gAppSurface, x1, y1, x2, y2, x2, y3, randomColor(), bounds);

	}
	//*/
}


void onLoop()
{
	background(0xffcccccc);
	drawRandomTriangles({0,0,canvasWidth,canvasHeight});
	refreshScreen();
}

void onLoad()
{
	setCanvasSize(800, 600);
}