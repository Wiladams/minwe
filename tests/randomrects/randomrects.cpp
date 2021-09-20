#include "apphost.h"
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


void background(PixelRGBA c)
{
	gAppSurface->setAllPixels(c);
}

void draw()
{
	//background(pLightGray);
	PixelRGBA stroke;
	PixelRGBA fill;

	for (int i = 1; i <= 1000; i++)
	{
		int x1 = random(canvasWidth - 1);
		int y1 = random(canvasHeight - 1);
		int lwidth = randomRange(4, 64);
		int lheight = randomRange(4, 64);
		int r = random(255);
		int g = random(255);
		int b = random(255);

		if (outlineOnly)
		{
			stroke = PixelRGBA(r, g, b, 255);
			//noFill();
			//GdSetMode(DPROP_COPY);
		}
		else
		{
			fill = PixelRGBA(r, g, b, 127);
			//noStroke();
			//GdSetMode(DPROP_SRC_OVER);
			fillRectangle(*gAppSurface, x1, y1, lwidth, lheight, fill);
		}

		//rect(x1, y1, lwidth, lheight);
	}
}

void onLoop()
{
	draw();
	screenRefresh();
}

void onLoad()
{
	setCanvasSize(640, 480);
}