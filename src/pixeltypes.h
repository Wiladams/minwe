#pragma once



#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable: 4201)  // nonstandard extension used : nameless struct/union
#endif


#include <array>
#include <cstdint>

#define INLINE inline

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

    // When you need a COLORREF to be compatible with 
    // some Windows GDI routines
    //uint32_t toCOLORREF() { return red | (green << 8) | (blue << 16); }
    // Convenience constructors
    //PixelRGBA(const ColorRgba& rhs) noexcept :red(rhs.red*255), blue(rhs.blue*255), green(rhs.green*255), alpha(rhs.alpha*255) {}
    //operator ColorRgba () { return { (double)red/255.0,(double)green/255.0,(double)blue/255.0,(double)alpha/255.0 }; }


    // Return a simple grayscale
    //uint8_t lum() { return static_cast<uint8_t>((float)red * 0.2225f + (float)green * 0.7154f + (float)blue * 0.0721f); }

};

struct PixelCoord {
    int x;
    int y;

    PixelCoord() noexcept = default;
    constexpr PixelCoord(const PixelCoord& other) noexcept = default;
    PixelCoord(const int x, const int y) noexcept :x(x), y(y) {}

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
    virtual  T operator()(double u, double v, double w, const PixelCoord& p) const
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

    uint32_t toLuminance(uint8_t r, uint8_t g, uint8_t b) const
    {
        return redfactor[r] + greenfactor[g] + bluefactor[b];
    }

    uint32_t toLuminance(const PixelRGBA p) const
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
