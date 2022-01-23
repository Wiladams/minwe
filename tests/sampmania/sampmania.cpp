
#include "gui.h"
#include "sampledraw2d.h"
#include "screensnapshot.h"
#include "perlintexture.h"

std::shared_ptr<ScreenSnapshot> screenSamp = nullptr;
std::shared_ptr< NoiseSampler> perlinSamp = nullptr;

double ts = 4;

void keyPressed(const KeyboardEvent& e)
{
	switch (e.keyCode) {
	case VK_UP:
		ts += 1;
		ts = maths::Clamp(ts, 0, canvasWidth);

		break;
	case VK_DOWN:
		ts -= 1;
		ts = maths::Clamp(ts, 0, canvasWidth);
		break;
	}
}

void onFrame()
{

	// draw a triangle using a screen snapshot as a texture
	screenSamp->next();
	sampleTriangle(*gAppSurface, 
		350, 200,
		700, 450,
		10, 450,
		*screenSamp,
		{0,0,canvasWidth, canvasHeight});

	// Trapezoid
	PixelCoord verts[] = { PixelCoord({800,10}),PixelCoord({1200,10}),PixelCoord({1900,700}),PixelCoord({710,700}) };
	int nverts = 4;
	sampleConvexPolygon(*gAppSurface, 
		verts, nverts, 0, 
		*screenSamp, 
		{ 0,0,canvasWidth, canvasHeight });

	// Rectangle
	//NoiseSampler noiseSamp(ts);
	//sampleRectangle(*gAppSurface, { 600,720,600,400 }, noiseSamp);

}

void setup()
{
	setCanvasSize(1920, 1080);
	setFrameRate(15);

	screenSamp = std::make_shared<ScreenSnapshot>(0, 150, 800, displayHeight / 2);
	perlinSamp = std::make_shared<NoiseSampler>(4);

	sampleRectangle(*gAppSurface, gAppSurface->frame(), *perlinSamp);

	// some 1D sampled horizontal lines
	RainbowSampler s(1.0);
	for (int counter = 1; counter <= 300; counter++) {
		sampleSpan(*gAppSurface, 10, 40+counter, 300+counter, s);
	}

}