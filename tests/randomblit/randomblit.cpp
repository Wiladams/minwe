#include "apphost.h"
#include "draw2d.h"
#include "gui.h"


User32PixelMap src(320, 240);

void setup()
{
	setCanvasSize(1024, 768);

	// create the source as a solid color
	src.setAllPixels(0xffffff00);

	// set the canvas to white
	gAppSurface->setAllPixels(0xffffffff);

	// Now do the blit
	blit(*gAppSurface, 10, 10, src);

	blit(*gAppSurface, 824, 10, src);

	blit(*gAppSurface, 824, 668, src);

	refreshScreen();
}