#pragma once

#include "apidefs.h"

#include <array>
#include <cstdint>


/*

In this case, PixelRGBA has 4 byte sized(8 - bit) values to represent
each of the R, G, B, A values.Some modern day monitorsand graphics
systems in general, are capable of using 10 - bit or even 16 - bit values.
As this is not universally common, we'll stick to 8-bit components
for now.

If you were to set a PixelRGBA by its integer value, it would layout
on a little endian system as follows :

0xAARRGGBB
*/
struct PixelRGBA 
{
    uint32_t value;

    // default constructor, transparent
    INLINE PixelRGBA() noexcept = default;
    INLINE constexpr PixelRGBA(const PixelRGBA& rhs) noexcept = default;
    INLINE constexpr explicit PixelRGBA(uint32_t val) noexcept : value(val) {}
    INLINE PixelRGBA(uint32_t r, uint32_t g, uint32_t b, uint32_t a = 0xFFu) noexcept : value((r << 16) | (g << 8) | b | (a << 24)) {}
    
    // bool operator for fast comparison to fully transparent
    INLINE explicit operator bool() const noexcept { return value != 0; }
    INLINE PixelRGBA& operator=(const PixelRGBA& other) noexcept = default;
    INLINE constexpr bool operator==(const PixelRGBA& other) const noexcept { return equals(other); }

    INLINE constexpr bool equals(const PixelRGBA& other) const noexcept { return value == other.value; }

    INLINE constexpr uint32_t r() const noexcept { return (value >> 16) & 0xffu; }
    INLINE constexpr uint32_t g() const noexcept { return (value >> 8) & 0xffu; }
    INLINE constexpr uint32_t b() const noexcept { return (value >> 0) & 0xffu; }
    INLINE constexpr uint32_t a() const noexcept { return (value >> 24) & 0xffu; }

    INLINE void setR(uint32_t r) noexcept { value = (value & 0xff00ffffu) | (r << 16); }
    INLINE void setG(uint32_t g) noexcept { value = (value & 0xffff00ffu) | (g << 8); }
    INLINE void setB(uint32_t b) noexcept { value = (value & 0xffffff00u) | (b << 0); }
    INLINE void setA(uint32_t a) noexcept { value = (value & 0x00ffffffu) | (a << 24); }

    INLINE constexpr bool isOpaque() const noexcept { return value >= 0xff000000u; }
    INLINE constexpr bool isTransparent() const noexcept { return value <= 0x00ffffff; }


};

// When you need a COLORREF to be compatible with 
// some Windows GDI routines
//uint32_t toCOLORREF() { return red | (green << 8) | (blue << 16); }
// Convenience constructors

// Return a simple grayscale
//uint8_t lum() { return static_cast<uint8_t>((float)red * 0.2225f + (float)green * 0.7154f + (float)blue * 0.0721f); }


struct PixelCoord {
    int x;
    int y;

    INLINE PixelCoord() noexcept = default;
    INLINE constexpr PixelCoord(const PixelCoord& other) noexcept = default;
    INLINE PixelCoord(int x, int y) noexcept :x(x), y(y) {}

    INLINE PixelCoord& operator=(const PixelCoord& other) noexcept = default;
};

struct PixelSize {
    int w;
    int h;
};

struct TextureCoord {
    double s;
    double t;
};

struct PixelSpan 
{
private:
    int fx;
    int fy;
    int fw;

public:
    INLINE PixelSpan() noexcept = default;
    INLINE constexpr PixelSpan(const PixelSpan& other) noexcept = default;
    INLINE PixelSpan(int x, int y, int w) noexcept :fx(x), fy(y), fw(w) {}

    INLINE PixelSpan& operator=(const PixelSpan& other) noexcept = default;

    INLINE constexpr int x() noexcept { return fx; }
    INLINE constexpr int y() noexcept { return fy; }
    INLINE constexpr int w() noexcept { return fw; }
};

struct PixelRect {
    int x;
    int y;
    int width;
    int height;

    PixelRect() : x(0), y(0), width(0), height(0) {}
    PixelRect(const int x, const int y, const int w, const int h)
        :x(x), y(y), width(w), height(h) {}

    INLINE constexpr bool isEmpty() const
    {
        return ((width <= 0) || (height <= 0));
    }

    INLINE constexpr bool containsPoint(const int x1, const int y1) const
    {
        if ((x1 < this->x) || (y1 < this->y))
            return false;

        if ((x1 >= this->x + this->width) || (y1 >= this->y + this->height))
            return false;

        return true;
    }

    INLINE constexpr bool containsRect(const PixelRect& other) const
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
    INLINE PixelRect intersection(const PixelRect& b) const
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

// Represents a rectangular area of a sampler
struct TexelRect 
{ 
    double left; 
    double top; 
    double right; 
    double bottom; 

    TexelRect() noexcept :left(0), top(0), right(0), bottom(0) {}
    INLINE constexpr TexelRect(const TexelRect& other) noexcept = default;
    INLINE TexelRect(double l, double t, double r, double b) noexcept
        :left(l), top(t), right(r), bottom(b) {}

    INLINE TexelRect& operator=(const TexelRect& other) noexcept = default;

    // This routine assumes the frame is within the constrained area
    static TexelRect create(const PixelRect& isect, const PixelRect& constraint)
    {
        // If no intersection, there's no need to figure
        // out texture coordinates
        if (isect.isEmpty())
            return TexelRect();

        // Figure out texture coordinates based on the intersection
        double left = maths::Map(isect.x, constraint.x, constraint.x + constraint.width - 1, 0, 1);
        double top = maths::Map(isect.y, constraint.y, constraint.y + constraint.height - 1, 0, 1);
        double right = maths::Map(isect.x + isect.width - 1, constraint.x, constraint.x + constraint.width - 1, 0, 1);
        double bottom = maths::Map(isect.y + isect.height - 1, constraint.y, constraint.y + constraint.height - 1, 0, 1);

        return  TexelRect(left, top, right, bottom);
    }

};

// The ISample interface is meant to support a generic interface
// for generating color values based on parameters.
// Doing this makes it easy to create color values for different
// rendering situations, without limiting ourselves to bitmaps.
//
// You can create a sample to return a certain type using the 
// teamplate class Sample<T>.
//
// struct SolidColorSample2D : ISample2D<PixelRGBA>
//

// A 1 dimensional sampler
template <typename T>
struct ISample1D
{
    virtual T getValue(double u, const PixelCoord& p) = 0;
    T operator()(double u, const PixelCoord& p) 
    {
        return getValue(u, p);
    }
};

// A 2 dimentional sampler
template <typename T>
struct ISample2D
{
    virtual T getValue(double u, double v, const PixelCoord& p) = 0;
    T operator()(double u, double v, const PixelCoord& p) 
    {
        return getValue(u, v, p);
    }
};

// A 3 dimensional sampler
template <typename T>
struct ISample3D
{
    virtual T getValue(double u, double v, double w, const PixelCoord& p) = 0;
    T operator()(double u, double v, double w, const PixelCoord& p)
    {
        return getValue(u, v, w, p);
    }
};

template <typename T>
struct ISampleRGBA :
    public ISample1D<T>,
    public ISample2D<T>,
    public ISample3D<T>
{

};


class NTSCGray
{
    // Based on old NTSC
    //static float redcoeff = 0.299f;
    //static float greencoeff = 0.587f;
    //static float bluecoeff = 0.114f;

    // New CRT and HDTV phosphors
    const float redcoeff = 0.2225f;
    const float greencoeff = 0.7154f;
    const float bluecoeff = 0.0721f;

    // use lookup tables to save ourselves from multiplications later
    std::array<uint8_t, 256> redfactor;
    std::array<uint8_t, 256> greenfactor;
    std::array<uint8_t, 256> bluefactor;

public:
    NTSCGray() :NTSCGray(0.2225f, 0.7154f, 0.0721f) {}

    // Create an instance, initializing with the coefficients you desire
    NTSCGray(float rcoeff, float gcoeff, float bcoeff)
        :redcoeff(rcoeff), greencoeff(gcoeff), bluecoeff(bcoeff)
    {
        // construct lookup tables
        for (int counter = 0; counter < 256; counter++)
        {
            redfactor[counter] = (uint8_t)maths::Min(56, (int)maths::Floor((counter * redcoeff) + 0.5f));
            greenfactor[counter] = (uint8_t)maths::Min(181, (int)maths::Floor((counter * greencoeff) + 0.5f));
            bluefactor[counter] = (uint8_t)maths::Min(18, (int)maths::Floor((counter * bluecoeff) + 0.5f));
        }
    }

    INLINE constexpr uint32_t toLuminance(uint8_t r, uint8_t g, uint8_t b) const
    {
        return redfactor[r] + greenfactor[g] + bluefactor[b];
    }

    INLINE constexpr uint32_t toLuminance(const PixelRGBA p) const
    {
        return redfactor[p.r()] + greenfactor[p.g()] + bluefactor[p.b()];
    }
};


// Some useful functions
// return a pixel value from a ISample2D based on the texture coordinates
// this is purely a convenience to match what you can do in OpenGL GLSL language
inline PixelRGBA texture2D(ISample2D<PixelRGBA> &tex0, const TextureCoord& st) noexcept
{
    return tex0.getValue(st.s, st.t, {});
}

// turn a division by 255 into something 
// much cheaper to calculate
// for values between 0 and 65534
#define div255(num) ((num + (num >> 8)) >> 8)

// Calculate the linear interpolation between two things
// these values should be between 0 and 255 inclusive
#define lerp255(bg, fg, a) ((uint8_t)div255((fg*a+bg*(255-a))))

#define blend_pixel(bg, fg) PixelRGBA(				\
	lerp255(bg.r(), fg.r(), fg.a()), \
	lerp255(bg.g(), fg.g(), fg.a()), \
	lerp255(bg.b(), fg.b(), fg.a()), 255)
