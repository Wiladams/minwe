#pragma once

/*
    This is a simple drawing library that has functions ranging from
    setting a single pixel all the way up to a triangle and ellipse.

    In general, the higher level functions will do bounds checking
    and deal with clipping primitives appropriately.

    The functions work against the PixelMap class.  They are not 
    the fastest routines possible, but they should be fairly complete.

    All operations are SRCCOPY.  It is relatively to add different
    pixel covering functions with these base routines.
*/

#include "pixelmap.h"
#include "maths.hpp"


// Set a single pixel
inline void point(PixelMap& pmap, const int x, const int y, const PixelRGBA src)
{
    // check for containment
    if ((x < 0) || (x >= pmap.width) || (y < 0) || (y >= pmap.height))
        return;

    pmap.set(x, y, src);
}


//
// Line Clipping in preparation for line drawing
//
typedef int OutCode;

static const int LN_INSIDE = 0; // 0000
static const int LN_LEFT = 1;   // 0001
static const int LN_RIGHT = 2;  // 0010
static const int LN_BOTTOM = 4; // 0100
static const int LN_TOP = 8;    // 1000

// Compute the bit code for a point (x, y) using the clip rectangle
// bounded diagonally by (xmin, ymin), and (xmax, ymax)
inline int computeOutCode(PixelRect rct, const int x, const int y)
{
    int code = LN_INSIDE;   // initialised as being inside of clip window
    int xmin = rct.x;
    int xmax = rct.x + rct.width - 1;
    int ymin = rct.y;
    int ymax = rct.y + rct.height - 1;
       

    if (x < xmin)           // to the left of clip window
        code |= LN_LEFT;
    else if (x > xmax)      // to the right of clip window
        code |= LN_RIGHT;
    
    if (y < ymin)           // below the clip window
        code |= LN_BOTTOM;
    else if (y > ymax)      // above the clip window
        code |= LN_TOP;

    return code;
}


// Using cohen Sutherland clipping algorithm
// Changes the input line coordinates to fit within
// the specified clipping rectangle.
// if the line is completely outside the clipping rectangle
// then false is returned, otherwise, true is returned
// and the input coordinates are changed to those that will
// fit within the clipping rectangle.
//
inline bool clipLine(const PixelRect bounds, int& x0, int& y0, int& x1, int& y1)
{
    // compute outcodes for P0, P1, and whatever point lies outside the clip rectangle
    int outcode0 = computeOutCode(bounds, x0, y0);
    int outcode1 = computeOutCode(bounds, x1, y1);
    bool accept = false;
    double xmin = bounds.x;
    double xmax = bounds.x + bounds.width - 1;
    double ymin = bounds.y;
    double ymax = bounds.y + bounds.height - 1;

    while (true) {
        if (!(outcode0 | outcode1)) { // Bitwise OR is 0. Trivially accept and get out of loop
            accept = true;
            break;
        }
        else if (outcode0 & outcode1) { // Bitwise AND is not 0. Trivially reject and get out of loop
            break;
        }
        else {
            // failed both tests, so calculate the line segment to clip
            // from an outside point to an intersection with clip edge
            double x = 0;
            double y = 0;

            // At least one endpoint is outside the clip rectangle; pick it.
            int outcodeOut = outcode0 ? outcode0 : outcode1;

            // Now find the intersection point;
            // use formulas y = y0 + slope * (x - x0), x = x0 + (1 / slope) * (y - y0)
            if (outcodeOut & LN_TOP) {           // point is above the clip rectangle
                x = x0 + (x1 - x0) * (ymax - y0) / (y1 - y0);
                y = ymax;
            }
            else if (outcodeOut & LN_BOTTOM) { // point is below the clip rectangle
                x = x0 + (x1 - x0) * (ymin - y0) / (y1 - y0);
                y = ymin;
            }
            else if (outcodeOut & LN_RIGHT) {  // point is to the right of clip rectangle
                y = y0 + (y1 - y0) * (xmax - x0) / (x1 - x0);
                x = xmax;
            }
            else if (outcodeOut & LN_LEFT) {   // point is to the left of clip rectangle
                y = y0 + (y1 - y0) * (xmin - x0) / (x1 - x0);
                x = xmin;
            }

            // Now we move outside point to intersection point to clip
            // and get ready for next pass.
            if (outcodeOut == outcode0) {
                x0 = x;
                y0 = y;
                outcode0 = computeOutCode(bounds, x0, y0);
            }
            else {
                x1 = x;
                y1 = y;
                outcode1 = computeOutCode(bounds, x1, y1);
            }
        }
    }

    return accept;
}

// Replicate the single pixel value across a horizontal line
// This assumes coordinates have already been clipped
inline void setSpan(PixelMap& pmap, const int x, const int y, const int width, const PixelRGBA src)
{
    /*
    // need to do line clipping
    int x1 = x;
    int y1 = y;
    int x2 = x1 + width - 1;
    int y2 = y;

    if (!clipLine({ 0,0,pmap.width, pmap.height }, x1, y1, x2, y2))
        return;

    int w = x2 - x1;
    */
    PixelRGBA* pixelPtr = (PixelRGBA*)pmap.getPixelPointer(x, y);
    __stosd((unsigned long*)pixelPtr, src.intValue, width);
}

inline void verticalLine(PixelMap& pmap, const int x, const int y, const int height, const PixelRGBA src)
{
    for (int row = y; row < y + height - 1; row++)
        pmap.set(x, row, src);
}

/*
strokeLine()

Stroke a line using the current stroking pixel.
Uses Bresenham line drawing.  Does not check for clipping,
but the underlying PixelBuffer might.

Note: an easy optimization would be to use the specialized
horizontal and vertical line drawing routines when necessary

BUGBUG - should sanity check line fits
within bounds
*/
inline void line(PixelMap& pmap, const int x1, const int y1, const int x2, const int y2, PixelRGBA c)
{
    int sdx, sdy, dxabs, dyabs;
    unsigned int x, y, px, py;

    // do clipping right here
    int x11 = x1;
    int y11 = y1;
    int x21 = x2;
    int y21 = y2;

    if (!clipLine({ 0,0,pmap.width, pmap.height }, x11, y11, x21, y21))
        return;

    int w = x21 - x11;

    int dx = x21 - x11;      // the horizontal distance of the line
    int dy = y21 - y11;      // the vertical distance of the line
    dxabs = maths::Abs(dx);
    dyabs = maths::Abs(dy);
    sdx = maths::Sign(dx);
    sdy = maths::Sign(dy);



    if (dyabs == 0) {
        // optimize for horizontal line
        x = x11 > x21 ? x21 : x11;
        setSpan(pmap, x, y11, dxabs, c);
        return;
    }

    x = dyabs >> 1;
    y = dxabs >> 1;
    px = x11;
    py = y11;

    pmap.set(x11, y11, c);  // set initial pixel

    if (dxabs >= dyabs) // the line is more horizontal than vertical
    {
            for (int i = 0; i<dxabs; i++)
            {
                y = y + dyabs;
                if (y >= (unsigned int)dxabs)
                {
                    y = y - dxabs;
                    py = py + sdy;
                }
                px = px + sdx;
                pmap.set(px, py, c);
            }
    } else // the line is more vertical than horizontal
    {
            for (int i = 0; i<dyabs; i++)
            {
                x += dxabs;
                if (x >= (unsigned int)dyabs)
                {
                    x -= dyabs;
                    px += sdx;
                }
                py += sdy;
                pmap.set(px, py, c);
            }
    }

}

// Draw the outline of a rectangle
inline void strokeRectangle(PixelMap& pmap, const int x, const int y, const int w, const int h, PixelRGBA c)
{
    // draw top and bottom
    setSpan(pmap, x, y, w, c);
    setSpan(pmap, x, y + h - 1, w, c);

    // draw sides
    verticalLine(pmap, x, y, h, c);
    verticalLine(pmap, x + w - 1, y, h, c);
}

// fill the inside of a rectangle
inline void fillRectangle(PixelMap &pmap, const int x, const int y, const int w, const int h, PixelRGBA c)
{
    for (int row = y; row < y + h; row++)
    {
        // Check for clip of each span
        int x1 = x;
        int y1 = row;
        int x2 = x + w - 1;
        int y2 = row;

        if (!clipLine({ 0,0,pmap.width, pmap.height }, x1, y1, x2, y2))
            continue;

        int newWidth = x2 - x1;

        setSpan(pmap, x1, y1, newWidth, c);
    }
}

/*
    Bresenham ellipse drawing algorithm
    Only for the frame, not for filling
*/
typedef void(*EllipseHandler)(PixelMap& pb, const int cx, const int cy, const int x, const int y, const PixelRGBA color);

inline void Plot4EllipsePoints(PixelMap& pb, const int cx, const int cy, const int x, const int y, const PixelRGBA color)
{
    int lowx = cx - x;
    int maxx = cx + x;
    int lowy = cy - y;
    int maxy = cy + y;

    point(pb, cx + x, cy + y, color);
    point(pb, cx - x, cy + y, color);
    point(pb, cx - x, cy - y, color);
    point(pb, cx + x, cy - y, color);

    /*
    if (containsPoint(pb->frame, maxx, maxy))
        pb_rgba_cover_pixel(pb, cx + x, cy + y, color);

    if (containsPoint(pb->frame, lowx, maxy))
        pb_rgba_cover_pixel(pb, cx - x, cy + y, color);

    if (containsPoint(pb->frame, lowx, lowy))
        pb_rgba_cover_pixel(pb, cx - x, cy - y, color);

    if (containsPoint(pb->frame, maxx, lowy))
        pb_rgba_cover_pixel(pb, cx + x, cy - y, color);
        */
}

inline void fill2EllipseLines(PixelMap& pb, const int cx, const int cy, const int x, const int y, const PixelRGBA color)
{
    int x1 = cx - x;
    int y1 = cy + y;
    int x2 = cx + x;
    int y2 = cy + y;

    if (clipLine({0,0,pb.width, pb.height}, x1, y1, x2, y2)) {
        setSpan(pb, x1, y1, x2 - x1, color);
    }

    y1 = cy - y;
    y2 = cy - y;
    if (clipLine({0,0,pb.width, pb.height}, x1, y1, x2, y2)) {
        setSpan(pb, x1, y1, x2 - x1, color);
    }

    //raster_rgba_hline_blend(pb, cx - x, cy - y, 2 * x, color);
}

inline void raster_ellipse(PixelMap& pb, const int cx, const int cy, const size_t xradius, size_t yradius, const PixelRGBA color, EllipseHandler handler)
{
    int x, y;
    int xchange, ychange;
    int ellipseerror;
    int twoasquare, twobsquare;
    int stoppingx, stoppingy;

    twoasquare = 2 * xradius * xradius;
    twobsquare = 2 * yradius * yradius;

    x = xradius;
    y = 0;

    xchange = yradius * yradius * (1 - 2 * xradius);
    ychange = xradius * xradius;
    ellipseerror = 0;
    stoppingx = twobsquare * xradius;
    stoppingy = 0;

    // first set of points, sides
    while (stoppingx >= stoppingy)
    {
        handler(pb, cx, cy, x, y, color);
        y++;
        stoppingy += twoasquare;
        ellipseerror += ychange;
        ychange += twoasquare;
        if ((2 * ellipseerror + xchange) > 0) {
            x--;
            stoppingx -= twobsquare;
            ellipseerror += xchange;
            xchange += twobsquare;
        }
    }

    // second set of points, top and bottom
    x = 0;
    y = yradius;
    xchange = yradius * yradius;
    ychange = xradius * xradius * (1 - 2 * yradius);
    ellipseerror = 0;
    stoppingx = 0;
    stoppingy = twoasquare * yradius;

    while (stoppingx <= stoppingy) {
        handler(pb, cx, cy, x, y, color);
        x++;
        stoppingx += twobsquare;
        ellipseerror += xchange;
        xchange += twobsquare;
        if ((2 * ellipseerror + ychange) > 0) {
            y--;
            stoppingy -= twoasquare;
            ellipseerror += ychange;
            ychange += twoasquare;
        }
    }
}

inline void strokeEllipse(PixelMap& pb, const int cx, const int cy, const size_t xradius, size_t yradius, const PixelRGBA color)
{
    raster_ellipse(pb, cx, cy, xradius, yradius, color, Plot4EllipsePoints);
}

inline void fillEllipse(PixelMap& pb, const int cx, const int cy, const size_t xradius, size_t yradius, const PixelRGBA color)
{
    raster_ellipse(pb, cx, cy, xradius, yradius, color, fill2EllipseLines);
}


/*
* Polygons, and triangles
    Triangles
*/

struct APolyDda {
    short vertIndex;
    short vertNext;
    float x;
    float dx;
    short ybeg;
    short yend;


    void setupPolyDda(const PixelCoord* pVerts, const int numVerts, short ivert, int dir)
    {
        vertIndex = ivert;
        vertNext = ivert + dir;
        if (vertNext < 0) {
            vertNext = numVerts - 1;
        }
        else if (vertNext == numVerts) {
            vertNext = 0;
        }

        // set starting/ending ypos and current xpos
        ybeg = yend;
        yend = round(pVerts[vertNext].y);
        x = pVerts[vertIndex].x;

        // Calculate fractional number of pixels to step in x (dx)
        float xdelta = pVerts[vertNext].x - pVerts[vertIndex].x;
        int ydelta = yend - ybeg;
        if (ydelta > 0) {
            dx = xdelta / ydelta;
        }
        else {
            dx = 0;
        }
    }
};

inline void fillTriangle(PixelMap& pb, const int x1, const int y1,
    const int x2, const int y2,
    const int x3, const int y3, PixelRGBA color, const PixelRect& clipRect)
{
    int nverts = 3;

    // Create a triangle object
    PixelTriangle tri(x1, y1, x2, y2, x3, y3);

    // find topmost vertex of the polygon
    int vmin = 0;

    // set starting line
    APolyDda ldda, rdda;
    int y = tri.verts[vmin].y;
    ldda.yend = rdda.yend = y;

    // setup polygon scanner for left side, starting from top
    ldda.setupPolyDda(tri.verts, nverts, vmin, +1);

    // setup polygon scanner for right side, starting from top
    rdda.setupPolyDda(tri.verts, nverts, vmin, -1);

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
            ldda.setupPolyDda(tri.verts, nverts, ldda.vertNext, +1);	// reset left side
        }

        // check for right dda hitting end of polygon side
        // if so, reset scanner
        if (y >= rdda.yend) {
            rdda.setupPolyDda(tri.verts, nverts, rdda.vertNext, -1);
        }

        // fill span between two line-drawers, advance drawers when
        // hit vertices
        if (y >= clipRect.y) {
            int rx = round(rdda.x);
            int lx = round(ldda.x);
            int w = abs(rx - lx);
            int x = lx < rx ? lx : rx;
            setSpan(pb, x, y, w, color);
            //line(pb, round(ldda.x), y, round(rdda.x), y, color);
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

/*
void fillConvexPolygon(PixelMap& pb, PixelCoord * verts, const int nverts, const DPCLIPRECT& clipRect, PixelRGBA color)
{
    // find topmost vertex of the polygon
    int vmin = findTopmostVertex(verts, nverts);

    // set starting line
    APolyDda ldda, rdda;
    int y = int(verts[vmin * 2 + 1]);
    ldda.yend = rdda.yend = y;

    // setup polygon scanner for left side, starting from top
    ldda.setupPolyDda(verts, nverts, vmin, +1);

    // setup polygon scanner for right side, starting from top
    rdda.setupPolyDda(verts, nverts, vmin, -1);

    //setColor(poly.colorNative);

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
            raster_rgba_hline_blend(pb, round(ldda.x), y, round(rdda.x) - round(ldda.x), color);
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
*/

inline void blit(PixelMap & pb, const int x, const int y, PixelMap & src)
{
    PixelRect bounds(0, 0, pb.width, pb.height);
    PixelRect srcFrame(x, y, src.width, src.height);

    PixelRect isect = bounds.intersection(srcFrame);
    int srcX = isect.x - x;
    int srcY = isect.y - y;

    uint32_t* dstPtr = (uint32_t *)pb.getPixelPointer(isect.x,isect.y);

    int rowCount = 0;
    for (int srcrow = srcY; srcrow < srcY+src.height; srcrow++)
    {
        uint32_t* srcPtr = nullptr;
        srcPtr = (uint32_t*)src.getPixelPointer(srcX, srcrow);
        memcpy(dstPtr, srcPtr, isect.width * 4);
        rowCount++;
        dstPtr = (uint32_t*)pb.getPixelPointer(isect.x, isect.y+rowCount);
    }
}
