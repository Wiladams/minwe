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

public:
    User32PixelMap()
    {
        x = 0;
        y = 0;
        width = 0;
        height = 0;
    }

    User32PixelMap(const long awidth, const long aheight)
    {
        x = 0;
        y = 0;
        width = awidth;
        height = aheight;
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
        x = 0;
        y = 0;
        width = awidth;
        height = aheight;
        int bitsPerPixel = 32;
        int alignment = 4;
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

    inline long getWidth() const { return width; }
    inline long getHeight() const { return height; }
    inline PixelRGBA* getData() { return (PixelRGBA*)fData; }
    inline PixelRGBA* getPixelPointer(const int x, const int y) {return &((PixelRGBA*)fData)[(y * getWidth()) + x]; }

    // Calculate whether a point is whithin our bounds
    inline bool contains(double x, double y) const { return ((x >= 0) && (x < width) && (y >= 0) && (y < height)); }

    inline BITMAPINFO getBitmapInfo() {return fBMInfo;}

    inline HDC getDC() {return fBitmapDC;}


    // Set a single pixel (srccopy)
    inline virtual void set(const int x, const int y, const PixelRGBA c)
    {
        // reject pixel if out of boundary
        if ((x < 0) || (x >= width) ||
            (y < 0) || (y >= height)) {
            return;
        }

        size_t offset = (size_t)(y * width) + (size_t)x;
        ((PixelRGBA*)fData)[offset] = c;
    }


    // set consecutive pixels in a row 
    inline void setPixels(const int x, const int y, const int width, const PixelRGBA src)
    {
        // do line clipping
        // copy actual pixel data
        uint32_t* pixelPtr = (uint32_t*)getPixelPointer(x, y);
        __stosd((unsigned long*)pixelPtr, src.intValue, width);
    }

    // Set every pixel to a specified value
    inline void setAllPixels(const PixelRGBA c)
    {
        size_t nPixels = width * height;
        __stosd((unsigned long*)fData, c.intValue, nPixels);
    }



    // Retrieve a single pixel
    inline virtual PixelRGBA get(const int x, const int y) const
    {
        // reject pixel if out of boundary
        if ((x < 0) || (x >= width) ||
            (y < 0) || (y >= height)) {
            return 0;    // return transparent pixel
        }

        // Get data from BLContext
        size_t offset = (size_t)(y * width) + (size_t)x;
        return ((PixelRGBA*)fData)[offset];
    }

    inline void rect(const int x, const int y, const int w, const int h, const PixelRGBA c)
    {
        for (int row = y; row < y + h; row++)
            setPixels(x, row, w, c);
    }

 };
