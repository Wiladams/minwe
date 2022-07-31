#include "apphost.h"
#include "draw.h"
#include "maths.hpp"

using namespace maths;

constexpr size_t MAXPOINTS = 200000;
PixelArray gpa;


void drawRandomPoints()
{
	for (size_t i = 0; i < MAXPOINTS; i++)
	{
		size_t x = random_int(canvasWidth-1);
		size_t y = random_int(canvasHeight-1);
		uint32_t gray = random_int(255);

		canvasPixels[(y * canvasWidth) + x] = PixelRGBA(gray, gray, gray);
	}
}

void onLoop()
{
	gAppSurface->setAllPixels(PixelRGBA(0x0));

	drawRandomPoints();
	
	refreshScreen();
}

void onLoad()
{
	setCanvasSize(800, 600);
	gpa.initArray(canvasPixels, canvasWidth, canvasHeight, canvasBytesPerRow);

	gAppWindow->moveTo(0, 0);
	layered();
}