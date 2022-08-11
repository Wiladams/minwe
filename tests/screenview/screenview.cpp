//
// screenview
// Simplest application to do continuous screen capture
// and display in another window.
//
#include "gui.h"

#include "screensnapshot.h"

ScreenSnapshot screenSamp;

void onFrame()
{
    // Get current screen snapshot
    screenSamp.next();

    // Draw a rectangle with snapshot as texture
    sampleRectangle(*gAppSurface,gAppSurface->frame(),screenSamp);
}

// Do application setup before things get
// going
void setup()
{
    // Setup application window
	setCanvasSize(displayWidth/2, displayHeight/2);
    setFrameRate(15);

    // setup the snapshot
    screenSamp.init(0, 0, displayWidth, displayHeight);
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