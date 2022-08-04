#include "apphost.h"
#include "draw2d.h"
#include "draw.h"

PixelRGBA randomColor()
{
	return { (uint32_t)random_int(255),(uint32_t)random_int(255), (uint32_t)random_int(255), (uint32_t)random_int(255) };
}


void drawRandomTriangles(PixelRect bounds)
{
	for (int i = 1; i <= 1000; i++)
	{
		int x1 = (int)random_int(-120, canvasWidth+10);
		int y1 = (int)random_int(-120, canvasHeight+10);
		int x2 = (int)random_int(x1 - 120, x1 + 120);
		int y2 = (int)random_int(y1+4, y1 + 120);
		int x3 = (int)random_int(x2 - 120, x2 + 120);
		int y3 = (int)random_int(y2+4, y2 + 120);

		fillTriangle(*gAppSurface, x1, y1, x2, y2, x2, y3, randomColor(), bounds);
	}
}

void onLoop()
{
	gAppSurface->setAllPixels(PixelRGBA(0xffcccccc));

	drawRandomTriangles({0,0,canvasWidth,canvasHeight});
	refreshScreen();
}

void onLoad()
{
	setCanvasSize(800, 600);
}