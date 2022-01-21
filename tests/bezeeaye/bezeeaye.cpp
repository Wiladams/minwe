
#include "gui.h"
#include "sampler.h"
#include "sampledraw2d.h"



int segments = 50;			// more segments make smoother curve
int dir = 1;				// which direction is the animation running
RainbowSampler s(1.0);		// Sample a rainbow of colors

constexpr int FRAMERATE = 15;
int currentIteration = 1;	// Changes during running
int iterations = 30;		// increase past frame rate to slow down


void drawRandomBezier(const PixelRect bounds)
{
	// clear to black
	gAppSurface->setAllPixels(PixelRGBA(0xff000000));

	// draw axis line
	line(*gAppSurface, bounds.x, bounds.y+bounds.height / 2, bounds.x+bounds.width, bounds.y + bounds.height / 2, PixelRGBA(0xffff0000));

	int x1 = bounds.x;
	int y1 = bounds.y + bounds.height / 2;

	int x2 = (int)maths::Map(currentIteration,1, iterations, 0, bounds.x + bounds.width - 1);
	int y2 = bounds.y;

	int x3 = (int)maths::Map(currentIteration, 1, iterations, bounds.x+bounds.width-1, 0);
	int y3 = bounds.y+bounds.height-1;

	int x4 = bounds.x+bounds.width-1;
	int y4 = bounds.y+bounds.height / 2;

	sampledBezier(*gAppSurface, x1, y1, x2, y2, x3, y3, x4, y4, segments, s);

	// Draw control lines
	line(*gAppSurface, x1, y1, x2, y2, PixelRGBA(0xffffffff));
	line(*gAppSurface, x2, y2, x3, y3, PixelRGBA(0xffffffff));
	line(*gAppSurface, x3, y3, x4, y4, PixelRGBA(0xffffffff));

	currentIteration += dir;
	
	// reverse direction if needs be
	if ((currentIteration >= iterations) || (currentIteration <= 1))
		dir = dir < 1 ? 1 : -1;
}


void setup()
{
	setCanvasSize(800, 600);
	setFrameRate(FRAMERATE);
}

void keyReleased(const KeyboardEvent& e) {
	switch (e.keyCode) {
	case VK_ESCAPE:
		halt();
		break;

	case VK_UP:
		iterations += 1;
		break;

	case VK_DOWN:
		iterations -= 1;
		if (iterations < 2)
			iterations = 2;
		break;
	}
}

void onFrame()
{
	drawRandomBezier({ 0,0,canvasWidth,canvasHeight });
}
