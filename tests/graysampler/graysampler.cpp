//
// graysampler
// Demonstrate the usage of the grayscale 2D sampler.
// This sampler wraps any other source sampler, and converts
// the color values to grayscale values.
//

#include "gui.h"
#include "rendercontext.h"
#include "screensnapshot.h"


std::shared_ptr<RenderContext> ctx = nullptr;
std::shared_ptr<ScreenSnapshot> screenSamp = nullptr;
std::shared_ptr<GrayscaleSampler> gSamp = nullptr;

void onFrame()
{
	// update screen snapshot
	screenSamp->next();

	// Draw a rectangle using grayscale sampler
	ctx->rectangle(0, 0, canvasWidth, canvasHeight, *gSamp);
}

void setup()
{
	setCanvasSize(1280, 1024);
	setFrameRate(30);

	// setup the snapshot
	screenSamp = std::make_shared<ScreenSnapshot>(0, 0, canvasWidth, canvasHeight);
	
	// setup the grayscale sampler
	gSamp = std::make_shared<GrayscaleSampler>(screenSamp);

	// A rendering context for convenience
	ctx = std::make_shared<RenderContext>(gAppSurface);
}
