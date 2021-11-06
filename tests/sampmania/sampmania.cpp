
#include "gui.h"
#include "sampledraw2d.h"
#include "screensnapshot.h"

std::shared_ptr<ScreenSnapshot> screenSamp = nullptr;


void onFrame()
{
	// draw a triangle using a screen snapshot as a texture
	screenSamp->next();
	sampleTriangle(*gAppSurface, 10, 300,
		700, 700,
		10, 700,
		*screenSamp,
		{0,0,canvasWidth, canvasHeight});

	// Trapezoid
	PixelCoord verts[] = { {800,10},{1200,10},{1900,700},{710,700} };
	int nverts = 4;
	sampleConvexPolygon(*gAppSurface, 
		verts, nverts, 0, 
		*screenSamp, 
		{ 0,0,canvasWidth, canvasHeight });
}

void setup()
{
	RainbowSampler s(1.0);
	screenSamp = std::make_shared<ScreenSnapshot>(100, 400, displayWidth / 4, displayHeight / 4);

	setCanvasSize(1920, 1080);
	setFrameRate(30);

	// some 1D sampled horizontal lines
	for (int counter = 1; counter <= 300; counter++) {
		sampleSpan(*gAppSurface, 10, 40+counter, 300+counter, s);
	}


}