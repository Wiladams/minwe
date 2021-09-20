#pragma once

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable: 4201)  // nonstandard extension used : nameless struct/union
#endif

/*
    A PixelMap is essentially a 2D array of pixels.  Pixels
    are a hard representation of a color value, suitable for display
    on the screen.  Pixels are RGBA values, not "color".  So they're not
    in a HSB, CYMK, or any other color space.  They're in the color space
    that's expected by the screen, or whatever is displaying the values.

    In this case, PixelRGBA has 4 byte sized (8-bit) values to represent
    each of the R,G,B,A values.  Some modern day monitors and graphics
    systems in general, are capable of using 10-bit or even 16-bit values.
    As this is not universally common, we'll stick to 8-bit components
    for now.

    If you were to set a PixelRGBA by its integer value, it would layout
    on a little endian system as follows:

    0xAARRGGBB
*/

typedef union PixelRGBA {
    uint32_t intValue;
    uint8_t data[4];
    struct {
        uint8_t blue, green, red, alpha;
    };

    // default constructor, transparent
    PixelRGBA() :intValue(0) {}

    // Copy constructor
    PixelRGBA(const PixelRGBA& rhs) :red(rhs.red), blue(rhs.blue), green(rhs.green), alpha(rhs.alpha) {}
    
    // Convenience constructors
    PixelRGBA(int r, int g, int b, int a) :red(r), green(g), blue(b), alpha(a) {}
    PixelRGBA(int r, int g, int b) :red(r), green(g), blue(b), alpha(255) {}
    PixelRGBA(uint32_t val) : intValue(val) {}

} PixelRGBA;

struct PixelPoint {
    int x;
    int y;

    PixelPoint() : x(0), y(0) {}
    PixelPoint(const int x, const int y) :x(x), y(y) {}

};

struct PixelRect {
    int x;
    int y;
    int width;
    int height;

    PixelRect() : x(0), y(0), width(0), height(0) {}
    PixelRect(const int x, const int y, const int w, const int h)
        :x(x), y(y), width(w), height(h) {}

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
class PixelMap
{
public:
    int width;
    int height;

    virtual PixelRGBA* getPixelPointer(const int x, const int y) =0;


    virtual void set(const int x, const int y, const PixelRGBA c)=0;
    virtual void setAllPixels(const PixelRGBA c) = 0;

    virtual PixelRGBA get(const int x, const int y) const = 0;
};

