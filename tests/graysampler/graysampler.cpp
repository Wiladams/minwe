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
std::shared_ptr<GraySampler> gSamp = nullptr;

void onFrame()
{
	screenSamp->next();

	double colWidth = canvasWidth / 3;

	// Draw a rectangle using grayscale sampler
	ctx->rect({ 0, 0, colWidth, (double)canvasHeight }, *gSamp, { 0,0,0.45,1 });
	ctx->rect({ colWidth, 0, colWidth, (double)canvasHeight }, *gSamp, { 0.45,0,0.55,1 });
	ctx->rect({ colWidth*2, 0, colWidth, (double)canvasHeight }, *gSamp, { 0.55,0,1,1 });

	ctx->rectangle(canvasWidth * .25, canvasHeight*0.25, canvasWidth * 0.5, canvasHeight*0.5, *screenSamp);
}

void setup()
{
	setCanvasSize(1290, 1024);
	//setFrameRate(30);

	// setup the snapshot
	screenSamp = std::make_shared<ScreenSnapshot>(0, 0, canvasWidth, canvasHeight);
	
	// setup the grayscale sampler
	gSamp = std::make_shared<GraySampler>(screenSamp);

	// A rendering context for convenience
	ctx = std::make_shared<RenderContext>(gAppSurface);
}
