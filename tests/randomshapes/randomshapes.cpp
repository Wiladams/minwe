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

inline PixelRGBA randomColor()
{
	return { (int)random(255), (int)random(255), (int)random(255) };
}

void drawRandomPoints(PixelRect r)
{

	for (int i = 0; i < 50000; i++)
	{
		auto c = randomColor();
		c.alpha = 210;
		gAppSurface->set(r.x + (int)random(r.width), r.y + (int)random(r.height), c);
	}
}

void drawRandomLines(PixelRect bounds)
{
	for (int i = 1; i <= 2500; i++)
	{
		int x1 = randomRange(bounds.x, bounds.x+bounds.width - 1);
		int y1 = randomRange(bounds.y, bounds.y+bounds.height - 1);
		int x2 = randomRange(bounds.x, bounds.x + bounds.width - 1);
		int y2 = randomRange(bounds.y, bounds.y+bounds.height - 1);
		auto c = randomColor();
		c.alpha = 210;

		line(*gAppSurface, x1, y1, x2, y2, c);
	}
}

void drawRandomRectangles(PixelRect bounds)
{
	PixelRGBA stroke;
	PixelRGBA fill;

	for (int i = 1; i <= 1000; i++)
	{
		int x1 = bounds.x + random(bounds.width - 4);
		int y1 = bounds.y+ random(bounds.height - 4);
		int lwidth = randomRange(4, bounds.x + bounds.width - 1- x1);
		int lheight = randomRange(4, bounds.y+bounds.height-1-y1);
		
		int r = random(255);
		int g = random(255);
		int b = random(255);

		fill = PixelRGBA(r, g, b, 127);
		fillRectangle(*gAppSurface, x1, y1, lwidth, lheight, fill);
	}
}

void drawRandomEllipses(PixelRect bounds)
{
	for (int i = 0; i < 1000; i++)
	{
		int cx = randomRange(bounds.x + 4, bounds.x + bounds.width - 1 - 4);
		int cy = randomRange(bounds.y + 4, bounds.y + bounds.height - 1 - 4);

		int xRadius = randomRange(4, 60);
		int yRadius = randomRange(4, 60);
		auto c = randomColor();
		c.alpha = 200;

		fillEllipse(*gAppSurface, cx, cy, xRadius, yRadius, c);
	}


}

void onFrame()
{
	drawRandomPoints({ 0,0,canvasWidth / 2, canvasHeight / 2 });
	drawRandomRectangles({ canvasWidth / 2, 0, canvasWidth / 2, canvasHeight / 2 });
	drawRandomLines({ 0,canvasHeight / 2,canvasWidth / 2,canvasHeight / 2 });
	drawRandomEllipses({ canvasWidth/2,canvasHeight/2,canvasWidth / 2, canvasHeight / 2 });

	refreshScreen();
}

void onLoop()
{
	onFrame();
}

void onLoad()
{
	setCanvasSize(800, 800);
	layered();
}
