#pragma once
// ScreenSnapshot
//
// Take a snapshot of a portion of the screen and hold
// it in a PixelMap (User32PixelMap)
// A sampler2D interface is also provided so you can 
// either use the pixel oriented 'get()' function, or the 
// parametric 'getValue()' function.
//
// When constructed, a single snapshot is taken.
// every time you want a new snapshot, just call 'next()'
// This is great for doing a live screen capture
//
//    ScreenSnapshot ss(x,y, width, height);
//
//    References:
//    https://www.codeproject.com/articles/5051/various-methods-for-capturing-the-screen
//

#include "User32PixelMap.h"
#include "sampler.h"

class ScreenSnapshot : public User32PixelMap, public ISample2D<PixelRGBA>
{
    HDC fScreenDC;  // Device Context for the screen

    // which location on the screen are we capturing
    int fOriginX;   
    int fOriginY;


public:
    ScreenSnapshot(int x, int y, int awidth, int aheight)
        : User32PixelMap(awidth, aheight),
        fOriginX(x),
        fOriginY(y)
    {
        // create a device context for the display
        fScreenDC = CreateDCA("DISPLAY", nullptr, nullptr, nullptr);
        
        // take at least one snapshot
        next();
    }

    // From sampler
    // Return a value based on the parametric (u,v) pair
    // 'p' is passed by the caller as a hint as to the 
    // absolute pixel value it's looking for
    virtual PixelRGBA getValue(double u, double v, const PixelCoord &p) const
    {
        int x = int((u * ((double)width() - 1))+0.5);
        int y = int((v * ((double)height() - 1))+0.5);

        return get(x, y);
    }

    // take a snapshot of current screen
    bool next()
    {
        BitBlt(getDC(), 0, 0, width(), height(), fScreenDC, fOriginX, fOriginY, SRCCOPY | CAPTUREBLT);

        return true;
    }
};