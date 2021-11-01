#pragma once

#include "draw2d.h"

// Use a sampler to fill in a span
// No clipping is performed
// This is really a specialization of a one
// line rectangle
inline void sampleSpan(PixelMap& pmap, const int x, const int y, const int width, ISample1D<PixelRGBA>& s)
{
    for (int col = x; col < x + width; col++)
    {
        double u = maths::Map(col, x, (double)x + width - 1, 0, 1);
        PixelRGBA c = s.getValue(u);
        pmap.set(col, y, c);
    }
}

//
// fill in a rectangle using the specified 
// 2D sampler.  This will deal with clipping
// so we don't overflow the buffer
inline void sampleRectangle(PixelMap& pmap, 
    const int x, const int y,
    const int w, const int h,
    ISample2D<PixelRGBA>& src)
{
    PixelRect bounds(pmap.x, pmap.y, pmap.width, pmap.height);
    PixelRect dstFrame(x, y, w, h);

    // find the intersection between the source rectangle
    // and the frame
    PixelRect dstisect = bounds.intersection(dstFrame);

    if (dstisect.isEmpty())
        return;

    int dstX = dstisect.x;
    int dstY = dstisect.y;

    int srcX = dstX - x;
    int srcY = dstY - y;

    for (int row = srcY; row < srcY + dstisect.height; row++)
    {
        for (int col = srcX; col < srcX + dstisect.width; col++)
        {
            double u = maths::Map(col, x, (double)x + w - 1, 0, 1);
            double v = maths::Map(row, y, (double)y + h - 1, 0, 1);
            auto c = src.getValue(u, v);
            pmap.set(col, row, c);
        }

    }
}

// 
// Draw a bezier line using a single line sampler
inline void sampledBezier(PixelMap& pmap, const int x1, const int y1, const int x2, const int y2,
    const int x3, const int y3, const int x4, const int y4, int segments, ISample1D<PixelRGBA>& c)
{
    PixelBezier bez(x1, y1, x2, y2, x3, y3, x4, y4);

    // Get starting point
    PixelCoord lp = bezier_point(0, bez.p1, bez.p2, bez.p3, bez.p4);

    int i = 1;
    while (i <= segments) {
        double u = (double)i / segments;

        PixelCoord p = bezier_point(u, bez.p1, bez.p2, bez.p3, bez.p4);

        // draw line segment from last point to current point
        line(pmap, lp.x, lp.y, p.x, p.y, c.getValue(u));

        // Assign current to last
        lp = p;

        i = i + 1;
    }
}

inline void sampleHLine2D(PixelMap& pb, int x1, int y1, int w,
    double v,ISample2D<PixelRGBA>& src)
{
    for (int x = x1; x < x1 + w - 1; x++)
    {
        double u = maths::Map(x, x1, x1 + w - 1, 0, 1);
        pb.set(x, y1, src.getValue(u, v));
    }
}

inline void sampleConvexPolygon(PixelMap& pb, 
    PixelCoord* verts, const int nverts, int vmin, 
    ISample2D<PixelRGBA>& src,
    const PixelRect& clipRect)
{
    // set starting line
    APolyDda ldda, rdda;
    int y = verts[vmin].y;
    ldda.yend = rdda.yend = y;

    // find lowest and highest y values
    int miny = 65535;
    int maxy = -65536;
    for (int i = 0; i < nverts; i++)
    {
        if (verts[i].y < miny)
            miny = verts[i].y;
        if (verts[i].y > maxy)
            maxy = verts[i].y;
    }

    // setup polygon scanner for left side, starting from top
    ldda.setupPolyDda(verts, nverts, vmin, +1);

    // setup polygon scanner for right side, starting from top
    rdda.setupPolyDda(verts, nverts, vmin, -1);

    while (true)
    {
        if (y >= ldda.yend)
        {
            if (y >= rdda.yend)
            {
                if (ldda.vertNext == rdda.vertNext) { // if same vertex, then done
                    break;
                }

                int vnext = rdda.vertNext - 1;

                if (vnext < 0) {
                    vnext = nverts - 1;
                }

                if (vnext == ldda.vertNext)
                {
                    break;
                }
            }
            ldda.setupPolyDda(verts, nverts, ldda.vertNext, +1);	// reset left side
        }

        // check for right dda hitting end of polygon side
        // if so, reset scanner
        if (y >= rdda.yend) {
            rdda.setupPolyDda(verts, nverts, rdda.vertNext, -1);
        }

        // fill span between two line-drawers, advance drawers when
        // hit vertices
        if (y >= clipRect.y) {
            int y1 = y;
            int y2 = y;
            int rx = round(rdda.x);
            int lx = round(ldda.x);
            int w = abs(rx - lx) + 1;
            int x1 = lx < rx ? lx : rx;
            int x2 = x1 + w - 1;

            if (clipLine({ 0,0,pb.width, pb.height }, x1, y1, x2, y2)) 
            {
                double v = maths::Map(y1, miny, maxy, 0, 1);
                w = x2 - x1;
                sampleHLine2D(pb, x1, y1, w, v, src);
            }
        }

        ldda.x += ldda.dx;
        rdda.x += rdda.dx;

        // Advance y position.  Exit if run off its bottom
        y += 1;
        if (y >= clipRect.y + clipRect.height)
        {
            break;
        }
    }
}


inline void sampleTriangle(PixelMap& pb, const int x1, const int y1,
    const int x2, const int y2,
    const int x3, const int y3, 
    ISample2D<PixelRGBA>& src, 
    const PixelRect& clipRect)
{
    // Create a triangle object
    PixelTriangle tri(x1, y1, x2, y2, x3, y3);

    // find topmost vertex of the polygon
    int nverts = 3;
    int vmin = 0;

    sampleConvexPolygon(pb, tri.verts, nverts, vmin, src, clipRect);
}
