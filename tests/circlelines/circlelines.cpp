#include "gui.h"

int radius = 400;
float angle = 0;
int cx = 400;
int cy = 400;
int increment = 6;

void keyReleased(const KeyboardEvent& e) {
	if (e.keyCode == VK_ESCAPE)
		halt();
}

void onFrame()
{
	// clear screen
	background(0xcc000000);

	int x = cx + (radius * cos(maths::Radians(angle)));
	int y = cy + (radius * sin(maths::Radians(angle)));

	fillEllipse(*gAppSurface, cx, cy, 8, 4, 0xff00ff00);
	line(*gAppSurface, cx, cy, x, y, 0xff00ffff);


	fillEllipse(*gAppSurface, x, y, 30, 30, 0xffff0000);

	angle += increment;
}

void setup()
{
	layered();
	setCanvasSize(800, 800);
	setFrameRate(30);
}