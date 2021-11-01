#pragma once

/*
    Take a snapshot of the video display using GDI.

    Construct an instance of the object by determining
    the location and size of the capture.  It is a fixed
    size and location, so can not be moved.  If you want
    to capture a different location, create another capture
    object.

    ScreenSnapshot ss(x,y, width, height);

    You can now use this as a 2D sampler
    Call next() whenever you want to take a snapshot

    ss.next()

    Then you can use getValue(u,v)

    References:
    https://www.codeproject.com/articles/5051/various-methods-for-capturing-the-screen
*/

#include "User32PixelMap.h"
#include "sampler.h"

class ScreenSnapshot : public ISample2D<PixelRGBA>
{
    HDC fScreenDC;
    User32PixelMap fSurface;

    int fOriginX;
    int fOriginY;
    int fWidth;
    int fHeight;

public:
    ScreenSnapshot(int x, int y, int awidth, int aheight)
        : fSurface(awidth, aheight),
        fOriginX(x),
        fOriginY(y),
        fWidth(awidth),
        fHeight(aheight)
    {
        // create a device context for the display
        fScreenDC = CreateDCA("DISPLAY", nullptr, nullptr, nullptr);
    }

    size_t width() { return fWidth; }
    size_t height() { return fHeight; }

    virtual PixelRGBA getValue(double u, double v) const
    {
        int x = int((u * ((double)fWidth - 1))+0.5);
        int y = int((v * ((double)fHeight - 1))+0.5);

        return fSurface.get(x, y);
    }

    // take a snapshot
    bool next()
    {
        BitBlt(fSurface.getDC(), 0, 0, fSurface.getWidth(), fSurface.getHeight(),
            fScreenDC, fOriginX, fOriginY, SRCCOPY | CAPTUREBLT);

        return true;
    }
};