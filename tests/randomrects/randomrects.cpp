
#include "gui.h"
#include "draw2d.h"

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

void keyReleased(const KeyboardEvent& e)
{
	if (e.keyCode == VK_ESCAPE)
		halt();
}

void onFrame()
{
	PixelRGBA stroke;
	PixelRGBA fill;
	PixelRGBA c;

	for (int i = 1; i <= 2000; i++)
	{
		int x1 = random(canvasWidth - 1);
		int y1 = random(canvasHeight - 1);
		//int lwidth = randomRange(4, 64);
		//int lheight = randomRange(4, 64);

		//int x1 = 10;
		//int y1 = 10;
		int lwidth = 100;
		int lheight = 100;

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


void setup()
{
	//fullscreen();
	setCanvasSize(800, 800);
	//layered();

	setFrameRate(100);
}