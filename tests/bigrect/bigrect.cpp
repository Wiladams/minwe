#include "gui.h"

// Test whether drawing a rectangle mostly offscreen will 
// be clipped correctly
void setup()
{
	setCanvasSize(800, 800);

	fillRectangle(*gAppSurface, 780, 780, 600, 600, PixelRGBA(0xffff0000));
}