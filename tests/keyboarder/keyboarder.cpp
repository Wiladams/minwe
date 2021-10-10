//#include "apphost.h"

#include "gui.h"
//#include "draw2d.h"

int leftX = 0;
int topY = 0;
int extentX = 120;
int extentY = 120;
int cx = 0;
int cy = 0;
int aperture = 120;

void keyReleased(const KeyboardEvent& e) {
    if (e.keyCode == VK_ESCAPE)
        halt();
}

void mouseMoved(const MouseEvent& e)
{
    cx = e.x;
    cy = e.y;
}

void mouseWheel(const MouseEvent& e)
{
    printf("wheel: %d\n", e.delta);
    if (e.delta > 0)
        aperture += 10;
    else
        aperture -= 10;

    if (aperture < 10)
        aperture = 10;
}

void onFrame()
{
    // clear screen first
    background(0xff1c0000);

    // draw horizontal line
    line(*gAppSurface, 0, cy-1, cx+aperture, cy-1, 0xffff0000);
    line(*gAppSurface, cx+aperture, 0, cx+aperture, cy+aperture, 0xffff0000);
    line(*gAppSurface, cx, cy+aperture, canvasWidth, cy+aperture, 0xffff0000);
    line(*gAppSurface, cx-1, cy, cx-1, canvasHeight, 0xffff0000);

    // Transparent
    fillRectangle(*gAppSurface, cx, cy, aperture, aperture, 0x01000000);

    //fillEllipse(*gAppSurface, cx, cy, aperture, aperture, 0x01000000);

    // Just a little targeting in the center
    //fillEllipse(*gAppSurface, cx, cy, 20, 20, 0xffffff00);

    //refreshScreen();
}

void setup()
{
    //setCanvasSize(800, 800);
    //layered();
    fullscreen();
}

