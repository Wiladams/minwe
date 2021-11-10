#pragma once

/*
    Simple representation of a bitmap we can use to draw into a window.
    We use a DIBSection because that's the easiest object within Windows 
    to get a pointer to the pixel data.

    we assume a format of 32-bit RGBA to make life very simple.

    A DC Context is also associated with the bitmap, to make it easier 
    to interact with other User32 and GDI interfaces.
*/

#include <windows.h>
#include <cstdio>
#include <intrin.h>

#include "bitbang.h"
#include "maths.hpp"
#include "pixelmap.h"

#pragma intrinsic(__stosd)


class User32PixelMap : public PixelMap
{
    // for interacting with win32
    BITMAPINFO fBMInfo{0};
    HBITMAP fDIBHandle = nullptr;
    HDC     fBitmapDC = nullptr;
    void * fData = nullptr;       // A pointer to the data
    size_t fDataSize=0;       // How much data is allocated
    
    // A couple of constants
    static const int bitsPerPixel = 32;
    static const int alignment = 4;

public:
    User32PixelMap()
    {
    }

    User32PixelMap(const long awidth, const long aheight)
        : PixelMap(0,0,awidth,aheight)
    {
        fDataSize = 0;

        init(awidth, aheight);
    }

    virtual ~User32PixelMap()
    {
        // BUGBUG
        // unload the dib section
        // and destroy it
    }

    virtual bool init(int awidth, int aheight)
    {
        fBounds = { 0,0,awidth,aheight };

        int bytesPerRow = winme::GetAlignedByteCount(awidth, bitsPerPixel, alignment);

        fBMInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
        fBMInfo.bmiHeader.biWidth = awidth;
        fBMInfo.bmiHeader.biHeight = -(LONG)aheight;	// top-down DIB Section
        fBMInfo.bmiHeader.biPlanes = 1;
        fBMInfo.bmiHeader.biBitCount = bitsPerPixel;
        fBMInfo.bmiHeader.biSizeImage = bytesPerRow * aheight;
        fBMInfo.bmiHeader.biClrImportant = 0;
        fBMInfo.bmiHeader.biClrUsed = 0;
        fBMInfo.bmiHeader.biCompression = BI_RGB;
        fDataSize = fBMInfo.bmiHeader.biSizeImage;

        // We'll create a DIBSection so we have an actual backing
        // storage for the context to draw into
        // BUGBUG - check for nullptr and fail if found
        fDIBHandle = ::CreateDIBSection(nullptr, &fBMInfo, DIB_RGB_COLORS, &fData, nullptr, 0);


        // Create a GDI Device Context
        fBitmapDC = ::CreateCompatibleDC(nullptr);

        // select the DIBSection into the memory context so we can 
        // peform operations with it
        ::SelectObject(fBitmapDC, fDIBHandle);

        // Do some setup to the DC
        ::SetBkMode(fBitmapDC, TRANSPARENT);
        ::SetGraphicsMode(fBitmapDC, GM_ADVANCED);

        return true;
    }

    inline PixelRGBA* getData() { return (PixelRGBA*)fData; }
    inline PixelRGBA* getPixelPointer(const int x, const int y) {return &((PixelRGBA*)fData)[(y * width()) + x]; }


    inline BITMAPINFO getBitmapInfo() {return fBMInfo;}

    inline HDC getDC() {return fBitmapDC;}


    // Set a single pixel value
    // Assume range checking has already occured
    inline virtual void setPixel(const int x, const int y, const PixelRGBA c)
    {
        size_t offset = (size_t)(y * fBounds.width) + (size_t)x;
        ((PixelRGBA*)fData)[offset] = c;
    }

    // set consecutive pixels in a row 
    // Assume the range has already been clipped
    inline void setPixels(const int x, const int y, const int width, const PixelRGBA src)
    {
        // do line clipping
        // copy actual pixel data
        uint32_t* pixelPtr = (uint32_t*)getPixelPointer(x, y);
        __stosd((unsigned long*)pixelPtr, src.intValue, width);
    }

    // Set every pixel to a specified value
    // we can use this fast intrinsic to fill
    // the whole area
    inline void setAllPixels(const PixelRGBA c)
    {
        size_t nPixels = fBounds.width * fBounds.height;
        __stosd((unsigned long*)fData, c.intValue, nPixels);
    }



    // Retrieve a single pixel
        // This one does no bounds checking, so the behavior is undefined
    // if the coordinates are beyond the boundary
    inline virtual PixelRGBA getPixel(const int x, const int y) const
    {
        // Get data from BLContext
        size_t offset = (size_t)(y * fBounds.width) + (size_t)x;
        return ((PixelRGBA*)fData)[offset];
    }

    // when checking bounds, return totally transparent
    // when outside bounds
    inline virtual PixelRGBA get(const int x, const int y) const
    {
        // reject pixel if out of boundary
        if (!fBounds.containsPoint(x, y))
            return 0;

        // Get data from BLContext
        size_t offset = (size_t)(y * fBounds.width) + (size_t)x;
        return ((PixelRGBA*)fData)[offset];
    }

 };
