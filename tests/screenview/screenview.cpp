#include "gui.h"
#include "sampledraw2d.h"
#include "screensnapshot.h"

std::shared_ptr<ScreenSnapshot> screenSamp = nullptr;

void onFrame()
{
    // Get current screen snapshot
    screenSamp->next();

    // Draw a rectangle with snapshot as texture
    sampleRectangle(*gAppSurface,
        200, 0,
        canvasWidth, canvasHeight,
        *screenSamp);
}
void setup()
{
    // setup the snapshot
    screenSamp = std::make_shared<ScreenSnapshot>(0, 0, displayWidth / 2, displayHeight/2);

    // Setup application window
	setCanvasSize(displayWidth/2, displayHeight/2);
    setFrameRate(30);
}