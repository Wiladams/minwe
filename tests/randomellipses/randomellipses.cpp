#include "apphost.h"
#include "draw2d.h"


inline PixelRGBA randomColor(int alpha=255)
{
	return { (int)maths::random(255), (int)maths::random(255), (int)maths::random(255), alpha };
}


void background(PixelRGBA c)
{
	gAppSurface->setAllPixels(c);
}

void drawRandomEllipses(PixelRect bounds)
{
	for (int i = 0; i < 1000; i++)
	{
		int cx = maths::randomRange(bounds.x+4, bounds.x+bounds.width-1-4);
		int cy = maths::randomRange(bounds.y + 4, bounds.y + bounds.height - 1 - 4);

		int xRadius = maths::randomRange(4, 60);
		int yRadius = maths::randomRange(4, 60);
		fillEllipse(*gAppSurface, cx, cy, xRadius, yRadius, randomColor(0xcd));
	}
}

void onLoop()
{
	background(0x00);
	drawRandomEllipses({ 0,0,canvasWidth,canvasHeight });
	refreshScreen();
}

void onLoad()
{
	setCanvasSize(1024, 768);
	layered();
}