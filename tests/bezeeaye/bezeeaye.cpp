

#include "gui.h"

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
	return { (int)random(255),(int)random(255), (int)random(255) };
}

int maxLines = 10;
int currentLine = 1;
int segments = 50;

void drawRandomBezier()
{

	// clear to black
	gAppSurface->setAllPixels(0xff000000);

	// draw axis line
	line(*gAppSurface, 0, canvasHeight / 2, canvasWidth, canvasHeight / 2, 0xffff0000);

	if (currentLine > maxLines)
		currentLine = 1;


	int x1 = 10;
	int y1 = 400;

	int x2 = (int)maths::Map(currentLine,1,maxLines, 10, 790);
	int y2 = 10;

	int x3 = (int)maths::Map(currentLine, 1, maxLines, 790, 10);
	int y3 = 790;

	int x4 = canvasWidth - 10;
	int y4 = 400;

		///auto c = randomColor();
		auto c = 0xff00ffff;

	bezier(*gAppSurface, x1, y1, x2, y2, x3, y3, x4, y4, segments, c);
	refreshScreen();

	currentLine += 1;

}

void onFrame()
{
	drawRandomBezier();
}

void setup()
{
	setCanvasSize(800, 800);
	setFrameRate(30);
}