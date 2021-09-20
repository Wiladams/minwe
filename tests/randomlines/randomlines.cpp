#include "apphost.h"
#include "draw2d.h"

#define cyan 0xff00ffff

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

void draw()
{
	float strokeWeight = 1.0;
	PixelRGBA stroke(0xff000000);
	PixelRGBA fill(0xffffffff);
	PixelRGBA background(0xffcccccc);

	// fill background with background color
	gAppSurface->setAllPixels(background);

	for (int i = 1; i <= 2500; i++)
	{
		int x1 = random(float(canvasWidth - 1));
		int y1 = random(float(canvasHeight - 1));
		int x2 = random(float(canvasWidth - 1));
		int y2 = random(float(canvasHeight - 1));

		int r = int(random(255));
		int g = int(random(255));
		int b = int(random(255));

		stroke = PixelRGBA(r, g, b, 255);
		fill = PixelRGBA(r, g, b, 255);

		strokeWeight = (1.0f);
		line(*gAppSurface, x1, y1, x2, y2, stroke);

		strokeWeight = 8.0f;
		point(*gAppSurface, x1, y1, stroke);
		point(*gAppSurface, x2, y2, stroke);
	}

	for (int x = 0; x < 400; x+=3)
	{
		line(*gAppSurface, x + 150, x, x + 450, x, cyan);
	}
}

void onLoop()
{
	draw();
	screenRefresh();
}

void onLoad()
{
	draw();
	screenRefresh();
}