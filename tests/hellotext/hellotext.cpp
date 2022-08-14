#include "gui.h"

constexpr int leftMargin = 24;
constexpr int topMargin = 24;


void drawTextDetail()
{
	// Text on another line
// Showing font metrics
	const char* str2 = "My Scene!";
	PixelCoord sz;
	textMeasure(sz, str2);

	//const int yCoord = 120;
	constexpr int myTop = 120;

	int baseline = myTop + fontHeight - fontDescent;
	int topline = myTop + fontLeading;


	strokeRectangle(*gAppSurface, leftMargin, myTop, sz.x(), sz.y(), PixelRGBA(0xffff0000));

	// Draw internalLeading - green
	copySpan(*gAppSurface, leftMargin, topline, sz.x(), PixelRGBA(0xff00ff00));

	// draw baseline
	copySpan(*gAppSurface, leftMargin, baseline, sz.x(), PixelRGBA(0xff0000ff));

	// Draw text in the box
	textColor(PixelRGBA(0xff00ffff));	// Turquoise Text
	text("My Scene!", leftMargin, myTop);
}

void setup()
{
	setCanvasSize(640, 280);
	background(PixelRGBA (0xffffffff));		// A white background

	// Some Red Text
	textFont("Sitka Text", 100);
	//textColor(PixelRGBA(0xffff0000));
	//text("Hello", leftMargin, topMargin);



	drawTextDetail();

	


}