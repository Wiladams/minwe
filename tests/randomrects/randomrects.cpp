#include "apphost.h"
#include "gui.h"
#include "draw2d.h"

#define pLightGray 0xffCCCCCC

bool outlineOnly = false;

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
	int r = random(255);
	int g = random(255);
	int b = random(255);

	return { r,g,b,255 };
}

void background(PixelRGBA c)
{
	gAppSurface->setAllPixels(c);
}

void draw()
{
	PixelRGBA stroke;
	PixelRGBA fill;
	PixelRGBA c;

	for (int i = 1; i <= 1000; i++)
	{
		int x1 = random(canvasWidth - 1);
		int y1 = random(canvasHeight - 1);
		int lwidth = randomRange(4, 64);
		int lheight = randomRange(4, 64);
		c = randomColor();

		if (outlineOnly)
		{
			stroke = c;
		}
		else
		{
			fill = c;
			fill.alpha = 127;
			fillRectangle(*gAppSurface, x1, y1, lwidth, lheight, fill);
		}
	}
}

void onLoop()
{
	draw();
	refreshScreen();
}

void setup()
{
	//fullscreen();
	setCanvasSize(800, 600);
}