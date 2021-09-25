#include "apphost.h"

#include "gui.h"
#include "draw2d.h"


void keyReleased(const KeyboardEvent& e) {
    if (e.keyCode == VK_ESCAPE)
        halt();
}

void mouseMoved(const MouseEvent& e)
{
    // clear screen first
    clearScreenTo(0xff000000);

    // draw horizontal line
    line(*gAppSurface, 0, e.y, canvasWidth, e.y, 0xffff0000);
    line(*gAppSurface, e.x, 0, e.x, canvasHeight, 0xffff0000);
    
    // Transparent
    fillEllipse(*gAppSurface, e.x, e.y, 120, 120, 0x01000000);

    // Just a little targeting in the center
    fillEllipse(*gAppSurface, e.x, e.y, 20, 20, 0xffffff00);

    refreshScreen();
}

void setup()
{
    fullscreen();
}

