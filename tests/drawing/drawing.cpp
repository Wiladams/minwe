#include "apphost.h"

// Some easy pixel values
#define black	PixelRGBA(0xff000000)
#define white	PixelRGBA(0xffffffff)
#define red		PixelRGBA(0xffff0000)
#define green	PixelRGBA(0xff00ff00)
#define blue	PixelRGBA(0xff0000ff)
#define yellow	PixelRGBA(0xffffff00)

// Return a pointer to a specific pixel in the array of
// canvasPixels
INLINE PixelRGBA* getPixelPointer(const int x, const int y) 
{ 
    return &((PixelRGBA*)canvasPixels)[(y * canvasWidth) + x]; 
}

// 
// Copy a pixel run as fast as we can
// to create horizontal lines.
// We do not check boundaries here.
// Boundary checks should be done elsewhere before
// calling this routine.  If you don't, you run the risk
// of running off the end of memory.
// The benefit is faster code possibly.
// This is the workhorse function for most other
// drawing primitives
INLINE void copyHLine(const size_t x, const size_t y, const size_t len, const PixelRGBA& c)
{
    unsigned long * dataPtr = (unsigned long*)getPixelPointer(x, y);
    __stosd(dataPtr, c.value, len);
}

// Draw a vertical line
// done as quickly as possible, only requiring an add
// between each pixel
// not as fast as HLine, because the pixels are not contiguous
// but pretty fast nonetheless.
INLINE void copyVLine(const size_t x, const size_t y, const size_t len, const PixelRGBA& c)
{
    size_t rowStride = canvasBytesPerRow;
    uint8_t * dataPtr = (uint8_t *)getPixelPointer(x, y);

    for (size_t counter = 0; counter < len; counter++)
    {
        *((PixelRGBA*)dataPtr) = c;
        dataPtr += rowStride;
    }
}

//
// create a rectangle by using copyHLine spans
// here we do clipping
INLINE void copyRectangle(const int x, const int y, const int w, const int h, const PixelRGBA &c)
{
    // We calculate clip area up front
    // so we don't have to do clipLine for every single line
    PixelRect dstRect = gAppSurface->frame().intersection({ x,y,w,h });

    // If the rectangle is outside the frame of the pixel map
    // there's nothing to be drawn
    if (dstRect.isEmpty())
        return;

    // Do a line by line draw
    for (int row = dstRect.y; row < dstRect.y + dstRect.height; row++)
    {
        copyHLine(dstRect.x, row, dstRect.width, c);
    }
}

// This gets called before the main application event loop
// gets going.
// The application framework calls refreshScreen() at least
// once after this, so we can do some drawing here to begin.
void onLoad()
{
	setCanvasSize(320, 240);
    gAppWindow->setTitle("drawing app");

	// clear screen to white
	gAppSurface->setAllPixels(white);

	copyRectangle(5, 5, 205, 205, yellow);

    copyHLine(5, 10, 205, red);

    copyHLine(5, 200, 205, blue);

    copyVLine(10, 5, 205, green);
    copyVLine(205, 5, 205, green);

}