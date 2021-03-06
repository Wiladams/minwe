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

PixelRGBA randomColor()
{
	uint32_t r = uint32_t(random_int(255));
	uint32_t g = uint32_t(random_int(255));
	uint32_t b = uint32_t(random_int(255));

	return { r,g,b,255 };
}

void draw()
{
	float strokeWeight = 1.0;
	PixelRGBA stroke(0xff000000);
	PixelRGBA fill(0xffffffff);
	//PixelRGBA background(0xffdccccc);
	PixelRGBA bkgpixel(0x0);

	// fill background with background color
	gAppSurface->setAllPixels(bkgpixel);


	for (int i = 1; i <= 10000; i++)
	{
		int x1 = random(float(canvasWidth - 1));
		int y1 = random(float(canvasHeight - 1));
		int x2 = random_int(x1-40,x1+40);
		int y2 = random_int(y1-40, y1+40);

		int r = random_int(255);
		int g = random_int(255);
		int b = random_int(255);

		stroke = PixelRGBA(r, g, b, 255);
		fill = PixelRGBA(r, g, b, 255);

		strokeWeight = (1.0f);
		line(*gAppSurface, x1, y1, x2, y2, stroke);

		strokeWeight = 8.0f;
		point(*gAppSurface, x1, y1, stroke);
		point(*gAppSurface, x2, y2, stroke);
	}

}

void onLoop()
{
	draw();

	refreshScreen();
}

void onLoad()
{
	setCanvasSize(displayWidth/2, displayHeight);
	//setCanvasSize(displayWidth, displayHeight);
	setWindowPosition(0, 0);
	layered();
}