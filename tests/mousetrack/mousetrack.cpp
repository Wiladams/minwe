/*
    Demonstration of how to subscribe
    to keyboard and mouse events.

    Using encapsulated drawing and PixelArray
*/
#include "apphost.h"
#include "draw.h"


// Some easy pixel values
#define black	PixelRGBA(0xff000000)
#define white	PixelRGBA(0xffffffff)
#define red		PixelRGBA(0xffff0000)
#define green	PixelRGBA(0xff00ff00)
#define blue	PixelRGBA(0xff0000ff)
#define yellow	PixelRGBA(0xffffff00)


// Just some glabal variables
int mouseX = 0;
int mouseY = 0;
int keyCode = -1;

// global pixel array (gpa)
// The array of pixels we draw into
// This will just wrap what's already created
// for the canvas, for convenience
PixelArray gpa;
PixelPolygon gpoly;
PixelPolygon gellipse1;
PixelPolygon gellipse2;

// For the application, we define the size of 
// the square we'll be drawing wherever the mouse is
constexpr size_t iconSize = 64;
constexpr size_t halfIconSize = 32;

// Define the initial color of the square we'll draw
// clicking on mouse, or space bar, will change color
PixelRGBA cColor(255, 0, 0);

// Simple routine to create a random color
PixelRGBA randomColor(uint32_t alpha = 255)
{
    return { (uint32_t)maths::random(255), (uint32_t)maths::random(255), (uint32_t)maths::random(255), alpha };
}

// function used to subscribe to keyboard events
// This routine will be called whenever there is a 
// keyboard event
void handleKeyboardEvent(const KeyboardEventTopic& p, const KeyboardEvent& e)
{
    keyCode = e.keyCode;

    switch (e.activity)
    {
        case KEYRELEASED:
            switch (keyCode) {
                case VK_ESCAPE:
                    halt();
                break;
                
                case VK_SPACE:
                    cColor = randomColor();
                break;
            }
        break;

    }
}

// This routine will be called whenever there
// is a mouse event in the application window
void handleMouseEvent(const MouseEventTopic& p, const MouseEvent& e)
{
    mouseX = e.x;
    mouseY = e.y;

    switch (e.activity)
    {
    case MOUSERELEASED:
        // change the color for the cursor
        cColor = randomColor();
        break;
    }
}

void drawLines(PixelArray &pa)
{
    // Draw some lines from the edge to where
    // the mouse is
    for (size_t x = 0; x < pa.width(); x += 4)
    {
        draw::copyLine(pa, x, 0, mouseX, mouseY, white);
    }

    for (size_t x = 0; x < pa.width(); x += 16)
    {
        draw::copyLine(pa, x, pa.height() - 1, mouseX, mouseY, white, 1);
    }

}

INLINE void createEllipse(PixelPolygon &poly, ptrdiff_t centerx, ptrdiff_t centery, ptrdiff_t xRadius, ptrdiff_t yRadius)
{
    static const int nverts = 72;
    int steps = nverts;

    ptrdiff_t awidth = xRadius * 2;
    ptrdiff_t aheight = yRadius * 2;

    for (size_t i = 0; i < steps; i++) {
        auto u = (double)i / steps;
        auto angle = u * (2 * maths::Pi);

        ptrdiff_t x = (int)std::floor((awidth / 2.0) * cos(angle));
        ptrdiff_t y = (int)std::floor((aheight / 2.0) * sin(angle));
        poly.addPoint(PixelCoord({ x + centerx, y + centery }));
    }
    poly.findTopmost();
}

// Each time through the main application 
// loop, do some drawing
void onLoop()
{
    // clear screen to black to start
    draw::copyAll(gpa, black);

    //draw::copyPolygon(gpa, gpoly, blue);

    drawLines(gpa);

    // draw a rectangle wherever the mouse is
    draw::copyRectangle(gpa, mouseX-halfIconSize, mouseY-halfIconSize, iconSize, iconSize, cColor);

    draw::copyPolygon(gpa, gellipse1, green);
    draw::copyPolygon(gpa, gellipse2, green);

    // force the canvas to be drawn
    refreshScreen();
}

// Called as the application is starting up, and
// before the main loop has begun
void onLoad()
{
    setTitle("mousetrack");

    // initialize the pixel array
    gpa.initArray(canvasPixels, canvasWidth, canvasHeight, canvasBytesPerRow);

    gpoly.addPoint(PixelCoord({ 20, 20 }));
    gpoly.addPoint(PixelCoord({ (ptrdiff_t)gpa.width() - 20, 20}));
    gpoly.addPoint(PixelCoord({ (ptrdiff_t)gpa.width() - 20, (ptrdiff_t)gpa.height() - 20}));
    gpoly.addPoint(PixelCoord({ (ptrdiff_t)20, (ptrdiff_t)gpa.height() - 20}));
    gpoly.findTopmost();

    createEllipse(gellipse1, 120, 120, 30, 30);
    createEllipse(gellipse2, (ptrdiff_t)gpa.width() - 120, 120, 30, 30);

    // setup to receive mouse and keyboard events
    subscribe(handleMouseEvent);
    subscribe(handleKeyboardEvent);
}