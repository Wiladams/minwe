#include "gui.h"

void setup()
{
	setCanvasSize(640, 280);
	background(PixelRGBA (0xffffffff));		// A white background

	textFont("Sitka Text", 100);
	textColor(PixelRGBA(0xffff0000));	// Red Text
	text("Hello", 0, 0);

	textColor(PixelRGBA(0xff0000ff));	// Blue Text
	text("My Scene!", 0, 100);
}