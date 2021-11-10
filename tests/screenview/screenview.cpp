//
// screenview
// Simplest application to do continuous screen capture
// and display in another window.
//
#include "gui.h"
#include "sampledraw2d.h"
#include "screensnapshot.h"

// placeholder for the screen snapshot thing
std::shared_ptr<ScreenSnapshot> screenSamp = nullptr;

void onFrame()
{
    // Get current screen snapshot

    // Draw a rectangle with snapshot as texture
    screenSamp->next();
    sampleRectangle(*gAppSurface,
        0, 0,canvasWidth, canvasHeight,
        *screenSamp);

    // This way doesn't work because you end up creating a new bitmap
    // and device context each time, and you'll exhaust those resources
    //ScreenSnapshot snapper(0, 0, displayWidth / 2, displayHeight / 2);
    //sampleRectangle(*gAppSurface,
    //    0, 0,canvasWidth, canvasHeight,
    //    snapper);

}

void setup()
{
    // setup the snapshot
    screenSamp = std::make_shared<ScreenSnapshot>(0, 0, displayWidth / 2, displayHeight/2);

    // Setup application window
	setCanvasSize(displayWidth/2, displayHeight/2);
    setFrameRate(30);
}