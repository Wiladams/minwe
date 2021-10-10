

#include "gui.h"


int iterations = 30;
int currentIteration = 1;
int segments = 50;
int dir = 1;

void keyReleased(const KeyboardEvent& e) {
	if (e.keyCode == VK_ESCAPE)
		halt();
}

void drawRandomBezier(const PixelRect bounds)
{
	// clear to black
	gAppSurface->setAllPixels(0xff000000);

	// draw axis line
	line(*gAppSurface, bounds.x, bounds.y+bounds.height / 2, bounds.x+bounds.width, bounds.y + bounds.height / 2, 0xffff0000);


	int x1 = bounds.x;
	int y1 = bounds.y + bounds.height / 2;

	int x2 = (int)maths::Map(currentIteration,1, iterations, 0, bounds.x + bounds.width - 1);
	int y2 = bounds.y;
	//int y2 = 100;

	int x3 = (int)maths::Map(currentIteration, 1, iterations, bounds.x+bounds.width-1, 0);
	int y3 = bounds.y+bounds.height-1;
	//int y3 = 100;

	int x4 = bounds.x+bounds.width-1;
	int y4 = bounds.y+bounds.height / 2;

	bezier(*gAppSurface, x1, y1, x2, y2, x3, y3, x4, y4, segments, 0xff00ffff);

	// Draw control lines
	line(*gAppSurface, x1, y1, x2, y2, 0xffffffff);
	line(*gAppSurface, x2, y2, x3, y3, 0xffffffff);
	line(*gAppSurface, x3, y3, x4, y4, 0xffffffff);

	currentIteration += dir;
	
	// reverse direction if needs be
	if ((currentIteration >= iterations) || (currentIteration <= 1))
		dir = dir < 1 ? 1 : -1;

}

void onFrame()
{
	drawRandomBezier({ 0,0,canvasWidth,canvasHeight });
}

void setup()
{
	setCanvasSize(800, 600);
	setFrameRate(30);
}