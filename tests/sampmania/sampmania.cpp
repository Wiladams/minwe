
#include "gui.h"
#include "sampledraw2d.h"
#include "screensnapshot.h"
#include "perlintexture.h"

ScreenSnapshot screenSamp;

void onFrame()
{
	// Take current snapshot of screen
	screenSamp.next();

	// Trapezoid
	PixelCoord verts[] = { PixelCoord({600,100}),PixelCoord({1000,100}),PixelCoord({1700,800}),PixelCoord({510,800}) };
	int nverts = 4;
	sampleConvexPolygon(*gAppSurface, 
		verts, nverts, 0, 
		screenSamp, 
		{ 0,0,canvasWidth, canvasHeight });

}

void setup()
{
	setCanvasSize(1920, 1080);
	setFrameRate(15);

	// Draw noisy background only once
	NoiseSampler perlinSamp(4);
	sampleRectangle(*gAppSurface, gAppSurface->frame(), perlinSamp);

	// Setup the screen sampler
	// Capture left half of screen
	screenSamp.init(0, 0, displayWidth / 2, displayHeight);
}

void keyReleased(const KeyboardEvent& e) {
	switch (e.keyCode)
	{
	case VK_ESCAPE:
		halt();
		break;

	case 'R':
	{
		recordingToggle();
	}
	break;
	}
}

/*
// draw a triangle using a screen snapshot as a texture
sampleTriangle(*gAppSurface,
	350, 200,
	700, 450,
	10, 450,
	screenSamp,
	{0,0,canvasWidth, canvasHeight});
	*/
