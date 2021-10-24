#include "gui.h"
#include "draw2d.h"

void setup()
{
	setCanvasSize(800, 800);

	fillRectangle(*gAppSurface, 780, 780, 600, 600, 0xffff0000);
	refreshScreen();
}