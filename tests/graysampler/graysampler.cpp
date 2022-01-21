//
// graysampler
// Demonstrate the usage of the grayscale 2D sampler.
// This sampler wraps any other source sampler, and converts
// the color values to grayscale values.
//

#include "gui.h"
#include "rendercontext.h"
#include "screensnapshot.h"
#include "sampledraw2d.h"

std::shared_ptr<RenderContext> ctx = nullptr;
std::shared_ptr<ScreenSnapshot> screenSamp = nullptr;
std::shared_ptr<LumaSampler> gSamp = nullptr;

void onFrame()
{
	screenSamp->next();

	sampleRect(*gAppSurface, PixelRect(0, 0, (double)canvasWidth, (double)canvasHeight), TexelRect( 0,0,1,1 ), *gSamp);
}

void setup()
{
	setCanvasSize(1290, displayHeight);
	setFrameRate(30);

	// setup the snapshot
	screenSamp = std::make_shared<ScreenSnapshot>(0, 0, canvasWidth, canvasHeight);
	
	// setup the grayscale sampler
	gSamp = std::make_shared<LumaSampler>(screenSamp);

	// A rendering context for convenience
	ctx = std::make_shared<RenderContext>(gAppSurface);
}
