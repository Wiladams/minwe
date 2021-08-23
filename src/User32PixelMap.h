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

#include "bitbang.h"
#include "maths.hpp"

typedef uint32_t Pixel;

class User32PixelMap
{
    // for interacting with win32
    BITMAPINFO fBMInfo{0};
    HBITMAP fDIBHandle = nullptr;
    HDC     fBitmapDC = nullptr;
    void * fData = nullptr;       // A pointer to the data
    size_t fDataSize=0;       // How much data is allocated
    long fWidth=0;
    long fHeight=0;


public:
    User32PixelMap(const long awidth, const long aheight)
        : fWidth(awidth),
        fHeight(aheight)
    {
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
    }

    virtual ~User32PixelMap()
    {
        // BUGBUG
        // unload the dib section
        // and destroy it
    }

    inline long getWidth() const { return fWidth; }
    inline long getHeight() const { return fHeight; }
    inline Pixel * getData() { return (Pixel *)fData; }

    // Calculate whether a point is whithin our bounds
    inline bool contains(double x, double y) const { return ((x >= 0) && (x < fWidth) && (y >= 0) && (y < fHeight)); }

    inline BITMAPINFO getBitmapInfo() {return fBMInfo;}

    inline HDC getDC() {return fBitmapDC;}


    inline void set(const int x, const int y, const Pixel c)
    {
        // reject pixel if out of boundary
        if ((x < 0) || (x >= fWidth) ||
            (y < 0) || (y >= fHeight)){
            return;
        }


        size_t offset = (size_t)(y * fWidth) + (size_t)x;
        ((Pixel*)fData)[offset] = c;
    }

    inline Pixel get(const int x, const int y) const
    {
        // reject pixel if out of boundary
        if ((x < 0) || (x >= fWidth) ||
            (y < 0) || (y >= fHeight)) {
            return 0;    // return transparent pixel
        }

        // Get data from BLContext
        size_t offset = (size_t)(y * fWidth) + (size_t)x;
        return ((Pixel *)fData)[offset];
    }

    inline void sync() const
    {
        // just sync the thing
    }

 };
