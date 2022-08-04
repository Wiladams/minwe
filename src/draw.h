#pragma once

#include "pixeltypes.h"
#include <algorithm>



struct draw
{

#define PIXELBLEND(bg, fg) PixelRGBA(				\
	lerp255(bg.r(), fg.r(), fg.a()), \
	lerp255(bg.g(), fg.g(), fg.a()), \
	lerp255(bg.b(), fg.b(), fg.a()), fg.a())

    static INLINE void pixel_copy(PixelArray& pa, const size_t x, const size_t y, const PixelRGBA& c)
    {
        ((PixelRGBA*)((uint8_t*)pa.data() + (y * pa.stride()) + (x * 4)))[0] = c;
    }

    static INLINE void vline_copy(PixelArray& pa, const size_t x, const size_t y, const size_t len, const PixelRGBA& c)
    {
        size_t rowStride = pa.stride();
        uint8_t* dataPtr = (uint8_t*)pa.pixelPointer(x, y);

        for (size_t counter = 0; counter < len; counter++)
        {
            *((PixelRGBA*)dataPtr) = c;
            dataPtr += rowStride;
        }
    }

    static INLINE void span_blend(PixelArray& pa, const size_t x, const size_t y, const size_t len, const PixelRGBA& src)
    {
        PixelRGBA* pixelPtr = (PixelRGBA*)pa.pixelPointer(x, y);
        for (int offset = 0; offset < len; offset++)
            pixelPtr[offset] = PIXELBLEND(pixelPtr[offset], src);
    }

    static INLINE void span_copy(PixelArray& pa, const size_t x, const size_t y, const size_t len, const PixelRGBA& c)
    {
        unsigned long* dataPtr = (unsigned long*)pa.pixelPointer(x, y);
        __stosd(dataPtr, c.value, len);
    }

    //
    // line()
    // Stroke a line using Bresenham line drawing.
    // clips line to frame of pixelmap
    //
    static INLINE void line_copy(PixelArray& pmap, ptrdiff_t x1, ptrdiff_t y1, ptrdiff_t x2, ptrdiff_t y2, const PixelRGBA& color, size_t width = 1)
    {
        const ptrdiff_t w = pmap.width() - 1;
        const ptrdiff_t h = pmap.height() - 1;

        if (x1 < 0 || y1 < 0 || x1 > w || y1 > h || x2 < 0 || y2 < 0 || x2 > w || y2 > h)
        {
            if ((x1 < 0 && x2 < 0) || (y1 < 0 && y2 < 0) || (x1 > w && x2 > w) || (y1 > h && y2 > h))
                return;

            if (y1 == y2)
            {
                x1 = std::min<ptrdiff_t>(std::max<ptrdiff_t>(x1, 0), w);
                x2 = std::min<ptrdiff_t>(std::max<ptrdiff_t>(x2, 0), w);
            }
            else if (x1 == x2)
            {
                y1 = std::min<ptrdiff_t>(std::max<ptrdiff_t>(y1, 0), h);
                y2 = std::min<ptrdiff_t>(std::max<ptrdiff_t>(y2, 0), h);
            }
            else
            {
                ptrdiff_t x0 = (x1 * y2 - y1 * x2) / (y2 - y1);
                ptrdiff_t y0 = (y1 * x2 - x1 * y2) / (x2 - x1);
                ptrdiff_t xh = (x1 * y2 - y1 * x2 + h * (x2 - x1)) / (y2 - y1);
                ptrdiff_t yw = (y1 * x2 - x1 * y2 + w * (y2 - y1)) / (x2 - x1);

                if (x1 < 0)
                {
                    x1 = 0;
                    y1 = y0;
                }
                if (x2 < 0)
                {
                    x2 = 0;
                    y2 = y0;
                }
                if (x1 > w)
                {
                    x1 = w;
                    y1 = yw;
                }
                if (x2 > w)
                {
                    x2 = w;
                    y2 = yw;
                }
                if ((y1 < 0 && y2 < 0) || (y1 > h && y2 > h))
                    return;

                if (y1 < 0)
                {
                    x1 = x0;
                    y1 = 0;
                }
                if (y2 < 0)
                {
                    x2 = x0;
                    y2 = 0;
                }

                if (y1 > h)
                {
                    x1 = xh;
                    y1 = h;
                }
                if (y2 > h)
                {
                    x2 = xh;
                    y2 = h;
                }
            }
        }

        const bool inverse = std::abs(y2 - y1) > std::abs(x2 - x1);
        if (inverse)
        {
            std::swap(x1, y1);
            std::swap(x2, y2);
        }

        if (x1 > x2)
        {
            std::swap(x1, x2);
            std::swap(y1, y2);
        }

        const double dx = double(x2 - x1);
        const double dy = (double)std::abs(y2 - y1);

        double error = dx / 2.0f;
        const ptrdiff_t ystep = (y1 < y2) ? 1 : -1;
        ptrdiff_t y0 = y1 - width / 2;

        for (ptrdiff_t x = x1; x <= x2; x++)
        {
            for (size_t i = 0; i < width; ++i)
            {
                ptrdiff_t y = y0 + i;
                if (y >= 0)
                {
                    if (inverse)
                    {
                        if (y < w)
                            draw::pixel_copy(pmap, y, x, color);
                    }
                    else
                    {
                        if (y < h)
                            draw::pixel_copy(pmap, x, y,color);
                    }
                }

            }

            error -= dy;
            if (error < 0)
            {
                y0 += ystep;
                error += dx;
            }
        }
    }

    static INLINE void rectangle_copy(PixelArray& pa, const int x, const int y, const int w, const int h, const PixelRGBA& c)
    {
        // We calculate clip area up front
        // so we don't have to do clipLine for every single line
        PixelRect dstRect = pa.frame().intersection({ x,y,w,h });

        // If the rectangle is outside the frame of the pixel map
        // there's nothing to be drawn
        if (dstRect.isEmpty())
            return;

        // Get pointer to data, and do fills,
        // and just jump row to row
        size_t rowStride = pa.stride();
        uint8_t* dataPtr = (uint8_t*)pa.pixelPointer(dstRect.x, dstRect.y);

        for (size_t counter = 0; counter < dstRect.height; counter++)
        {
            __stosd((unsigned long*)dataPtr, c.value, dstRect.width);
            dataPtr += rowStride;
        }
    }

    static INLINE void rectangle_blend(PixelArray& pa, const int x, const int y, const int w, const int h, const PixelRGBA& c)
    {
        // We calculate clip area up front
        // so we don't have to do clipLine for every single line
        PixelRect dstRect = pa.frame().intersection({ x,y,w,h });

        // If the rectangle is outside the frame of the pixel map
        // there's nothing to be drawn
        if (dstRect.isEmpty())
            return;

        // Get pointer to data, and do fills,
        // and just jump row to row
        size_t rowStride = pa.stride();
        uint8_t* dataPtr = (uint8_t*)pa.pixelPointer(dstRect.x, dstRect.y);

        for (size_t counter = 0; counter < dstRect.height; counter++)
        {
            __stosd((unsigned long*)dataPtr, c.value, dstRect.width);
            dataPtr += rowStride;
        }
    }

    static INLINE void copyAll(PixelArray& pa, const PixelRGBA& c)
    {
        draw::rectangle_copy(pa, 0, 0, pa.width(), pa.height(), c);
    }

    static INLINE void polygon_copy(PixelArray& pb, const PixelPolygon& poly, const PixelRGBA& c)
    {
        for (ptrdiff_t y = poly.fTop; y < poly.fBottom; ++y)
        {
            std::vector<ptrdiff_t> intersections;
            for (size_t i = 0; i < poly.fVertices.size(); ++i)
            {
                const PixelCoord& p0 = (i ? poly.fVertices[i - 1] : poly.fVertices.back()), p1 = poly.fVertices[i];
                if ((y >= p0.y() && y < p1.y()) || (y >= p1.y() && y < p0.y()))
                    intersections.push_back(p0.x() + (y - p0.y()) * (p1.x() - p0.x()) / (p1.y() - p0.y()));
            }
            assert(intersections.size() % 2 == 0);
            std::sort(intersections.begin(), intersections.end());
            for (size_t i = 0; i < intersections.size(); i += 2)
            {
                ptrdiff_t left = std::max<ptrdiff_t>(0, intersections[i + 0]);
                ptrdiff_t right = std::min<ptrdiff_t>(pb.width(), intersections[i + 1]);

                draw::span_copy(pb, left, y, right - left, c);
            }
        }

    }

    // This is a straight up pixel copy
    // no scaling, no alpha blending
    // it will deal with clipping so we don't
    // crash when going off the edges
    INLINE void blit_copy(PixelArray& pb, const int x, const int y, PixelArray& src)
    {
        PixelRect bounds(0, 0, pb.width(), pb.height());
        PixelRect dstFrame(x, y, src.width(), src.height());

        // Intersection of boundary and destination frame
        PixelRect dstisect = bounds.intersection(dstFrame);

        if (dstisect.isEmpty())
            return;

        int dstX = dstisect.x;
        int dstY = dstisect.y;

        int srcX = dstX - x;
        int srcY = dstY - y;

        // we're trying to avoid knowing the internal details of the
        // pixel maps, so we use getPixelPointer() to get a pointer
        // realistically, the blit should be implemented in PixelMap
        uint32_t* dstPtr = (uint32_t*)pb.pixelPointer(dstX, dstY);

        int rowCount = 0;
        for (int srcrow = srcY; srcrow < srcY + dstisect.height; srcrow++)
        {
            uint32_t* srcPtr = (uint32_t*)src.pixelPointer(srcX, srcrow);
            memcpy(dstPtr, srcPtr, dstisect.width * 4);
            rowCount++;
            dstPtr = (uint32_t*)pb.pixelPointer(dstisect.x, dstisect.y + rowCount);
        }
    }

};
