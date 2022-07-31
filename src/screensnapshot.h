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
//    https://stackoverflow.com/questions/5069104/fastest-method-of-screen-capturing-on-windows
//  https://github.com/bmharper/WindowsDesktopDuplicationSample
//

#include "User32PixelMap.h"
#include "sampler.h"

class ScreenSnapshot : public User32PixelMap
{
    HDC fSourceDC;  // Device Context for the screen

    // which location on the screen are we capturing
    int fOriginX;   
    int fOriginY;


public:
    ScreenSnapshot()
        : fSourceDC(nullptr)
        , fOriginX(0)
        , fOriginY(0)
    {}

    ScreenSnapshot(int x, int y, int awidth, int aheight, HDC srcDC = NULL)
        : User32PixelMap(awidth, aheight),
        fOriginX(x),
        fOriginY(y)
    {
        init(x, y, awidth, aheight, NULL);

        // take at least one snapshot
        next();
    }

    bool init(int x, int y, int awidth, int aheight, HDC srcDC=NULL)
    {
        User32PixelMap::init(awidth, aheight);

        if (NULL == srcDC)
            fSourceDC = GetDC(nullptr);
        else
            fSourceDC = srcDC;

        fOriginX = x;
        fOriginY = y;

        return true;
    }

    // take a snapshot of current screen
    bool next()
    {
        // copy the screendc into our backing buffer
        // getDC retrieves the device context of the backing buffer
        // which in this case is the 'destination'
        // the fSourceDC is the source
        // the width and height are dictated by the width() and height() 
        // and the source origin is given by fOriginX, fOriginY
        // We use the parameters (SRCCOPY, CAPTUREBLT) because that seems 
        // to be best practice in this case
        BitBlt(getDC(), 0, 0, width(), height(), fSourceDC, fOriginX, fOriginY, SRCCOPY | CAPTUREBLT);

        return true;
    }
};