
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

PixelRGBA randomColor(uint32_t alpha=255)
{
	uint32_t r = random(255);
	uint32_t g = random(255);
	uint32_t b = random(255);

	return { r,g,b,alpha };
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
	background(PixelRGBA(0));

	for (int i = 1; i <= 2000; i++)
	{
		int x1 = random(canvasWidth - 1);
		int y1 = random(canvasHeight - 1);
		//int lwidth = randomRange(4, 40);
		//int lheight = randomRange(4, 40);
		int lwidth = 60;
		int lheight = 60;

		c = randomColor(255);

		if (outlineOnly)
		{
			stroke = c;
			strokeRectangle(*gAppSurface, x1, y1, lwidth, lheight, stroke);
		}
		else
		{
			fill = c;
			fillRectangle(*gAppSurface, x1, y1, lwidth, lheight, fill);
		}
	}
}


void setup()
{
	//fullscreen();
	setCanvasSize(800, 800);
	layered();

	setFrameRate(100);
}