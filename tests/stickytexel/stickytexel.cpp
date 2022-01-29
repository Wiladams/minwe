#include "gui.h"
#include "screensnapshot.h"
#include "sampledraw2d.h"
#include "normalizedwindow.h"

#include <vector>

// 
// Trying to get the math right on Sub-samples
// We want a SubSample to be able to grab a portion of
// a wrapped sampler, and display it using the range
// [0..1]
//
// Extra tricky part is having a sub-sample as a feeder for
// another sub-sample


std::shared_ptr<ScreenSnapshot> screenCapture = nullptr;
std::shared_ptr< SubSample> screenCap1 = nullptr;
std::shared_ptr< SubSample> screenCap2 = nullptr;
std::shared_ptr< SampledWindow> windowMan = nullptr;

std::shared_ptr< SampledWindow> win1 = nullptr;
std::shared_ptr< SampledWindow> win2 = nullptr;

void setup()
{
	setCanvasSize(displayWidth, displayHeight);
	setFrameRate(30);

	windowMan = std::make_shared<SampledWindow>();

	// Capture the entire screen
	screenCapture = std::make_shared<ScreenSnapshot>(0, 0, displayWidth, displayHeight);
	
	// Create a couple of sub-samples of the screen capture
	screenCap1 = std::make_shared<SubSample>(screenCapture, TexelRect(0, 0, 0.499, 1.0), TexelRect(0, 0, 1, 1));
	screenCap2 = std::make_shared<SubSample>(screenCapture, TexelRect(0.50,0,1.0,1.0), TexelRect(0, 0, 1, 1));

	win1 = std::make_shared<SampledWindow>(screenCap1);
	win2 = std::make_shared<SampledWindow>(screenCap2);

	// Move the sub-samples around
	//subsamp1->setFrame(TexelRect(0.5, 0, 1.0, 1.0));
	//subsamp2->setFrame(TexelRect(0, 0, 0.499,1));

	//windowMan->addChild(subsamp2);
	//windowMan->addChild(subsamp1);
	//windowMan->addChild(screenCap1);
	//windowMan->addChild(screenCap2);
}

// Displaying a frame with the Normalized Window manager
// becomes very easy.  Just display the whole window manager
// because it's a sampler.  This is a great piece for 
// composition, as you can feed samplers into other samplers
// so, samplers all the way down.
void onFrame()
{
	screenCapture->next();

	// Take the capture, and put it where we want it
	//sampleRect(*gAppSurface, PixelRect(0, 0, (canvasWidth / 2), canvasHeight), TexelRect(0,0,1,1), *screenCap1);
	//sampleRect(*gAppSurface, PixelRect((canvasWidth / 2) + 1, 0, (canvasWidth / 2), canvasHeight), TexelRect(0, 0, 1, 1), *screenCap2);

	sampleRect(*gAppSurface, PixelRect(canvasWidth/2, 0, canvasWidth/2, canvasHeight), TexelRect(0, 0, 1, 1) , *screenCap1);
	sampleRect(*gAppSurface, PixelRect(0, 0, (canvasWidth/2), canvasHeight), TexelRect(0, 0, 1, 1), *screenCap2);

	//sampleRect(*gAppSurface, PixelRect(0, 0, canvasWidth/2, canvasHeight), TexelRect(0, 0, 1, 1), *win1);
	//sampleRect(*gAppSurface, PixelRect((canvasWidth / 2) + 1, 0, (canvasWidth/2), canvasHeight), TexelRect(0, 0, 1, 1), *win2);


	//sampleRectangle(*gAppSurface, PixelRect(0, 0, canvasWidth, canvasHeight), *windowMan);
}