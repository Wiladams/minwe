
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
	uint32_t r = random_int(255);
	uint32_t g = random_int(255);
	uint32_t b = random_int(255);

	return { r,g,b,alpha };
}

void keyReleased(const KeyboardEvent& e)
{
	if (e.keyCode == VK_ESCAPE)
		halt();

	if (e.keyCode == VK_SPACE)
		outlineOnly = !outlineOnly;
}

void onFrame()
{
	PixelRGBA stroke;
	PixelRGBA fill;
	PixelRGBA c;
	background(PixelRGBA(0));

	for (int i = 1; i <= 2000; i++)
	{
		int x1 = random_int(canvasWidth - 1);
		int y1 = random_int(canvasHeight - 1);
		int lwidth = random_int(4, 60);
		int lheight = random_int(4, 60);

		c = randomColor(127);

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
	//layered();

	setFrameRate(30);
}