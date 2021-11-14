#pragma once



#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable: 4201)  // nonstandard extension used : nameless struct/union
#endif


#include <array>

using byte = uint8_t;

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



//
// Color representation
// Really this should be some CIE generic representation
//
struct ColorRgba {
    double red=0.0f;
    double green = 0.0f;
    double blue = 0.0f;
    double alpha = 0.0f;

    ColorRgba() {}
    ColorRgba(const uint8_t r, const uint8_t g, const uint8_t b) :red((double)r / 255.0), green((double)g / 255.0), blue((double)b / 255.0),alpha(1.0) {}
    ColorRgba(double r, double g, double b, double a) :red(r), green(g), blue(b), alpha(a) {}
    ColorRgba(double r, double g, double b) :red(r), green(g), blue(b), alpha(1.0) {}

    ColorRgba operator * (double s) { return { red * s, green * s, blue * s, alpha }; }
    ColorRgba operator / (double s) { return { red / s, green / s, blue / s, alpha }; }
    ColorRgba operator + (const ColorRgba &rhs) { return { red + rhs.red, green +rhs.green, blue + rhs.blue, 1.0 }; }
    ColorRgba& operator += (const ColorRgba& rhs) {
        red += rhs.red;
        green += rhs.green;
        blue += rhs.blue;
        alpha += 1;
        return *this;  
    }
};

typedef union PixelRGBA {
    uint32_t intValue;
    uint8_t data[4];
    struct {
        uint32_t blue : 8;
        uint32_t green : 8;
        uint32_t red : 8;
        uint32_t alpha : 8;
    };

    // default constructor, transparent
    PixelRGBA() :intValue(0) {}

    // Copy constructor
    PixelRGBA(const PixelRGBA& rhs) :red(rhs.red), blue(rhs.blue), green(rhs.green), alpha(rhs.alpha) {}
    PixelRGBA(const ColorRgba& rhs) :red(rhs.red*255), blue(rhs.blue*255), green(rhs.green*255), alpha(rhs.alpha*255) {}

    // Convenience constructors
    PixelRGBA(int r, int g, int b, int a) :red(r), green(g), blue(b), alpha(a) {}
    PixelRGBA(int r, int g, int b) :red(r), green(g), blue(b), alpha(255) {}
    PixelRGBA(uint32_t val) : intValue(val) {}

    // When you need a COLORREF to be compatible with 
    // some Windows GDI routines
    operator ColorRgba () { return { (double)red/255.0,(double)green/255.0,(double)blue/255.0,(double)alpha/255.0 }; }

    uint32_t toCOLORREF() { return red | (green << 8) | (blue << 16); }
    uint8_t lum() { return static_cast<uint8_t>((float)red * 0.2225f + (float)green * 0.7154f + (float)blue * 0.0721f); }

} PixelRGBA;

struct PixelCoord {
    int x;
    int y;

    PixelCoord() : x(0), y(0) {}
    PixelCoord(const int x, const int y) :x(x), y(y) {}

};

struct TextureCoord {
    double s;
    double t;
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
    virtual T getValue(double u, const PixelCoord& p) const = 0;
    virtual T operator()(double u, const PixelCoord& p) const
    {
        return getValue(u, p);
    }
};

// A 2 dimentional sampler
template <typename T>
struct ISample2D
{
    virtual T getValue(double u, double v, const PixelCoord& p) const = 0;
    virtual T operator()(double u, double v, const PixelCoord& p) const
    {
        return getValue(u, v, p);
    }
};

// A 3 dimensional sampler
template <typename T>
struct ISample3D
{
    virtual T getValue(double u, double v, double w, const PixelCoord& p) const = 0;
    virtual T operator()(double u, double v, double w, const PixelCoord& p) const
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

    uint8_t toLuminance(uint8_t r, uint8_t g, uint8_t b) const
    {
        return redfactor[r] + greenfactor[g] + bluefactor[b];
    }

    uint8_t toLuminance(const PixelRGBA p) const
    {
        return redfactor[p.red] + greenfactor[p.green] + bluefactor[p.blue];
    }
};


// Some useful functions
// return a pixel value from a ISample2D based on the texture coordinates
// this is purely a convenience to match what you can do in OpenGL GLSL language
inline PixelRGBA texture2D(ISample2D<PixelRGBA> &tex0, const TextureCoord& st) noexcept
{
    return tex0.getValue(st.s, st.t, {});
}
