/*
    This widget demonstrates using transparency as well
    as some fun resizing control with mouse and keyboard
*/
#include "gui.h"

#include <list>
#include <array>

static const int minExtent = 120;
static const int targetExtent = 40;

int leftX = 0;
int topY = 0;
int extentX = minExtent;
int extentY = minExtent;
int cx = 0;
int cy = 0;
bool showTarget = false;

std::array<PixelLine, 4> borderLines;

// Position of lines within the list
enum {
    topLine     = 1,
    rightLine   = 2,
    bottomLine  = 4,
    leftLine    = 8
};

PixelRect target;

// given a couple of endpoints, return the 
// line generalized line equation
bool calculateLine(const PixelCoord pt1, const PixelCoord pt2, int &a, int &b, int &c)
{
    a = pt1.y - pt2.y;
    b = pt2.x - pt1.x;
    c = (pt1.x * pt2.y) - (pt2.x * pt1.y);

    if (a == 0 && b == 0)
        return false;

    return true;
}

double distanceToLine(PixelCoord pt, PixelCoord pt1, PixelCoord pt2)
{
    int a, b, c = 0;
    calculateLine(pt1, pt2, a, b, c);
    return abs(a * pt.x + b * pt.y + c) / sqrt(a * a + b * b);
}

// Recalculate border lines
void calcBorderLines()
{
    borderLines[0] = { { 0,cy - 1 },{cx + extentX, cy - 1} };             // top
    borderLines[1] = { {cx - 1,cy},{cx - 1,canvasHeight} };               // left
    borderLines[2] = { {cx,cy + extentY},{canvasWidth,cy + extentY} };    // bottom
    borderLines[3] = { {cx + extentX,0},{cx + extentX,cy + extentY} };    // right
}

void calcTarget()
{
    target.x = cx - (targetExtent / 2);
    target.y = cy - (targetExtent / 2);
    target.width = targetExtent;
    target.height = targetExtent;
}

void setup()
{
    fullscreen();

    // setup border lines
    calcBorderLines();
    calcTarget();
}

void onFrame()
{
    PixelRGBA controlLineColor(0xffff0000);

    // clear screen first
    background(0xcc1c1c1c);

    calcBorderLines();

    // draw border lines
    for (int i = 0; i < borderLines.size(); i++)
    {
        const auto &it = borderLines[i];
        line(*gAppSurface, it.pt1.x, it.pt1.y, it.pt2.x, it.pt2.y, controlLineColor);
    }


    // Draw Transparent window
    fillRectangle(*gAppSurface, cx, cy, extentX, extentY, 0x01000000);


    if (showTarget) {
        // draw mouse target
        fillRectangle(*gAppSurface, target.x, target.y, target.width, target.height, 0xffff0000);
    }
}

//
// Key presses will be continuous if you hold
// a key down.  So, smooth motion can be achieved
// if you just hold the key down.
//
// Holding SHIFT at the same time as an arrow key
// will cause expansion and contraction, rather than movement
//
void keyPressed(const KeyboardEvent& e) {
    SHORT res = GetAsyncKeyState(VK_SHIFT);
    bool isShift = res & 0x8000;  // high order bit indicates keypress state
                                    // low order bit indicates previous state

    switch (e.keyCode)
    {
    case VK_LEFT:
        if (isShift)
            extentX -= 10;
        else
            cx -= 10;
        break;

    case VK_RIGHT:
        if (isShift)
            extentX += 10;
        else
            cx += 10;
    break;

    case VK_UP:
        if (isShift)
            extentY -= 10;
        else
            cy -= 10;
        break;

    case VK_DOWN:
        if (isShift)
            extentY += 10;
        else
            cy += 10;
        break;
    }
}

void keyReleased(const KeyboardEvent& e) {
    switch (e.keyCode)
    {
        case VK_ESCAPE:
            halt();
            break;
    }
}

void mousePressed(const MouseEvent& e)
{
    if (e.lbutton) {
        cx = e.x;
        cy = e.y;
    }

    calcTarget();

}

void mouseMoved(const MouseEvent& e)
{
    // If the left button is being pressed
    // then move the viewport
    // while hovering, see if we're close to any 
    // of the border lines
    if (e.lbutton) {
        cx = e.x;
        cy = e.y;
    }

    calcTarget();
}

// Mouse Vertical wheel
void mouseWheel(const MouseEvent& e)
{
    // Regular mouse wheel, change vertical size
    //printf("wheel: %d\n", e.delta);
    int dir = 1;
    if (e.delta > 0)
        dir = -1;

    extentY += dir * 10;
    if (extentY < minExtent)
        extentY = minExtent;

}

// Mouse horizontal wheel
void mouseHWheel(const MouseEvent& e)
{
    // contrary to documentation
    // positive delta, rotated left
    // negative delta, rotated right
    // delta in increments of 30

    int dir = 1;
    int delta = (abs(e.delta) /30)+1;
    printf("H wheel: %d delta: %d\n", e.delta, delta);
    
    if (e.delta > 0)
        dir = -1;

    extentX += dir * delta * 10;
    if (extentX < minExtent)
        extentX = minExtent;

}