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

// Use this intrinsic for fast memory copies
#pragma intrinsic(__stosd)


class User32PixelMap : public PixelMap
{
    // for interacting with win32
    BITMAPINFO fBMInfo{0};
    HBITMAP fDIBHandle = nullptr;
    HGDIOBJ fOriginDIBHandle = nullptr;
    HDC     fBitmapDC = nullptr;
    void * fData = nullptr;       // A pointer to the data
    size_t fDataSize=0;       // How much data is allocated
    size_t fBytesPerRow = 0;        // Row stride

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
        //::SelectObject(fBitmapDC, fOriginDIBHandle);


        // and destroy it
        ::DeleteObject(fDIBHandle);
    }

    bool init(int awidth, int aheight)
    {
        fFrame = { 0,0,awidth,aheight };

        fBytesPerRow = winme::GetAlignedByteCount(awidth, bitsPerPixel, alignment);

        fBMInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
        fBMInfo.bmiHeader.biWidth = awidth;
        fBMInfo.bmiHeader.biHeight = -(LONG)aheight;	// top-down DIB Section
        fBMInfo.bmiHeader.biPlanes = 1;
        fBMInfo.bmiHeader.biBitCount = bitsPerPixel;
        fBMInfo.bmiHeader.biSizeImage = fBytesPerRow * aheight;
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
        fOriginDIBHandle = ::SelectObject(fBitmapDC, fDIBHandle);

        // Do some setup to the DC to make it suitable
        // for drawing with GDI if we choose to do that
        ::SetBkMode(fBitmapDC, TRANSPARENT);
        ::SetGraphicsMode(fBitmapDC, GM_ADVANCED);

        return true;
    }

    // Make sure all GDI drawing, if any, has completed
    void flush()
    {
        ::GdiFlush();
    }

    INLINE BITMAPINFO getBitmapInfo() { return fBMInfo; }
    INLINE HDC getDC() { return fBitmapDC; }
    INLINE PixelRGBA* getData() { return (PixelRGBA*)fData; }
    INLINE PixelRGBA* getPixelPointer(const int x, const int y) {return &((PixelRGBA*)fData)[(y * width()) + x]; }
    INLINE size_t bytesPerRow() const { return fBytesPerRow; }

    // Retrieve a single pixel
    // This one does no bounds checking, so the behavior is undefined
    // if the coordinates are beyond the boundary
    INLINE PixelRGBA getPixel(const int x, const int y) const
    {
        // Get data from BLContext
        size_t offset = (size_t)(y * width()) + (size_t)x;
        return ((PixelRGBA*)fData)[offset];
    }


    // Set a single pixel value
    // Assume range checking has already occured
    // Perform SRCCOPY operation on a pixel
    INLINE void copyPixel(const int x, const int y, const PixelRGBA &c) override
    {
        size_t offset = (size_t)(y * width()) + (size_t)x;
        ((PixelRGBA*)fData)[offset] = c;
    }

    // Perform SRCOVER operation on a pixel
    INLINE void blendPixel(const int x, const int y, const PixelRGBA &c) override
    {
        size_t offset = (size_t)(y * width()) + (size_t)x;
        ((PixelRGBA*)fData)[offset] = blend_pixel(((PixelRGBA*)fData)[offset],c);
    }

    // set consecutive pixels in a row 
    // Assume the range has already been clipped
    INLINE void setPixels(const int x, const int y, const int w, const PixelRGBA src)
    {
        // do line clipping
        // copy actual pixel data
        uint32_t* pixelPtr = (uint32_t*)getPixelPointer(x, y);
        __stosd((unsigned long*)pixelPtr, src.value, w);
    }

    // Set every pixel to a specified value
    // we can use this fast intrinsic to fill
    // the whole area
    INLINE void setAllPixels(const PixelRGBA &c)
    {
        size_t nPixels = width() * height();
        __stosd((unsigned long*)fData, c.value, nPixels);
    }

    /*
    INLINE void setAllPixels(const uint32_t value)
    {
        size_t nPixels = width() * height();
        __stosd((unsigned long*)fData, value, nPixels);
    }
    */

 };
