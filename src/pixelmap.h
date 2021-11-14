#pragma once

#include "glsl.h"

/*
    A PixelMap is essentially a 2D array of pixels.  Pixels
    are a hard representation of a color value, suitable for display
    on the screen.  Pixels are RGBA values, not "color".  So they're not
    in a HSB, CYMK, or any other color space.  They're in the color space
    that's expected by the screen, or whatever is displaying the values.
*/

// Representation of a line segment
struct PixelLine {
    PixelCoord pt1;
    PixelCoord pt2;
};

struct PixelRect {
    int x;
    int y;
    int width;
    int height;

    PixelRect() : x(0), y(0), width(0), height(0) {}
    PixelRect(const int x, const int y, const int w, const int h)
        :x(x), y(y), width(w), height(h) {}

    bool isEmpty() const
    {
        return ((width <= 0) || (height <= 0));
    }

    bool containsPoint(const int x1, const int y1) const
    {
        if ((x1 < this->x) || (y1 < this->y))
            return false;

        if ((x1 >= this->x + this->width) || (y1 >= this->y + this->height))
            return false;

        return true;
    }

    bool containsRect(const PixelRect& other) const
    {
        if (!containsPoint(other.x, other.y))
        {
            return false;
        }

        if (!containsPoint(other.x + other.width - 1, other.y + other.height - 1))
        {
            return false;
        }

        return true;
    }

    // return the intersection of rectangles a and b
// if there is no intersection, one or both of width and height
// will be == zero
    PixelRect intersection(const PixelRect& b) const
    {
        int x = this->x > b.x ? this->x : b.x;
        int y = this->y > b.y ? this->y : b.y;
        int right = ((this->x + this->width) < (b.x + b.width)) ? (this->x + this->width) : (b.x + b.width);
        int bottom = ((this->y + this->height) < (b.y + b.height)) ? (this->y + this->height) : (b.y + b.height);

        int width = ((right - x) > 0) ? (right - x) : 0;
        int height = ((bottom - y) > 0) ? (bottom - y) : 0;

        return{ x, y, width, height };

    }

};

/*
  PixelTriangle

  A triangle representation
*/
struct PixelTriangle {
    PixelCoord verts[3];   // A triangle has 3 vertices

    // Basic constructor take coordinates in any order, sorts them
    // so that subsequent drawing will be faster.
    PixelTriangle(const int x1, const int y1,
        const int x2, const int y2,
        const int x3, const int y3)
    {
        verts[0] = { x1, y1 };
        verts[1] = { x2, y2 };
        verts[2] = { x3, y3 };

        // sort the coordinates from topmost
        // to bottommost, so drawing is easier
        // This is essentially a 3 element bubble sort
        PixelCoord tmp;
        if (verts[0].y > verts[1].y) {
            tmp = verts[0];
            verts[0] = verts[1];
            verts[1] = tmp;
        }

        if (verts[1].y > verts[2].y) {
            tmp = verts[1];
            verts[1] = verts[2];
            verts[2] = tmp;
        }

        // One more round to ensure the second isn't the smallest
        if (verts[0].y > verts[1].y) {
            tmp = verts[0];
            verts[0] = verts[1];
            verts[1] = tmp;
        }
    }
};

// Simple description of an ellipse
struct PixelEllipse
{
    int cx, cy;
    int rx, ry;
};


// Cubic Bezier, defined by 4 points
struct PixelBezier
{
        PixelCoord p1;
        PixelCoord p2;
        PixelCoord p3;
        PixelCoord p4;
        size_t fSegments;

    public:
        PixelBezier(int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4, size_t segments = 60)
            :p1(x1, y1),
            p2(x2, y2),
            p3(x3, y3),
            p4(x4, y4),
            fSegments(segments)
        {
        }

};

/*
    PixelMap is an abstraction for a 2D representation of Pixel values.
    We want this abstraction layer so that we can implement different
    forms of PixelMaps without changing the API for how to interact 
    with them.

    The most essential functions related to a PixelMap are being able
    to set and get pixel values.  Beyond that, there might be some
    convenience functions for setting big chunks of pixels, like
    filling the whole map to a single color, or drawing
    lines.  This leaves space for implementations to optimize
    these functions in ways that are specific to them.
*/
class PixelMap : public ISample2D<PixelRGBA>
{
protected:
    PixelRect fBounds;

public:
    PixelMap() :fBounds(0,0,0,0){}

    // virtual destructor so base classes setup properly
    PixelMap(int x, int y, int w, int h)
        :fBounds(x,y,w,h) {}

    virtual ~PixelMap() {}

    // Things a sub-class MUST implement
    virtual bool init(int w, int h) = 0;
    virtual PixelRGBA* getPixelPointer(const int x, const int y) = 0;
    virtual size_t bytesPerRow() const = 0;
    virtual void setPixel(const int x, const int y, const PixelRGBA c) = 0;
    virtual void setAllPixels(const PixelRGBA c) = 0;
    virtual PixelRGBA getPixel(const int x, const int y) const = 0;

    // regular things
    inline int x() const { return fBounds.x; }
    inline int y() const { return fBounds.y; }
    inline int width() const { return fBounds.width; }
    inline int height() const { return fBounds.height; }
    
    // Calculate whether a point is whithin our bounds
    inline bool contains(double x, double y) const { return fBounds.containsPoint((int)x, (int)y); }

    const PixelRect& getBounds() const { return fBounds; }

    //
    // set(), and get() are the general purpose ways to get and set
    // pixel values.  They will do bounds checking.  If you want 
    // to avoid bounds checking,then use the getPixel(), and setPixel() forms
    // which MUST be implemented by a sub-class
    virtual void set(const int x, const int y, const PixelRGBA c) {
        if (!fBounds.containsPoint(x, y))
            return;

        setPixel(x, y, c);
    }

    // when checking bounds, return totally transparent
    // when outside bounds
    PixelRGBA get(const int x, const int y) const
    {
        // reject pixel if out of boundary
        if (!fBounds.containsPoint(x, y))
            return 0;

        return getPixel(x, y);

    }

    uint8_t getLuminance(const int x, const int y) const
    {

    }

    // ISample2D<PixelRGBA>
    PixelRGBA getValue(double u, double v, const PixelCoord& p) const
    {
        int x = int((u * ((double)width() - 1)) + 0.5);
        int y = int((v * ((double)height() - 1)) + 0.5);

        return get(x, y);
    }
};

