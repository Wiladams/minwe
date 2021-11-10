#pragma once

#include "pixelmap.h"
#include "coloring.h"

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
	virtual T operator()(double u, const PixelCoord &p) const
    {
        return getValue(u,p);
    }
};

// A 2 dimentional sampler
template <typename T>
struct ISample2D
{
    virtual T getValue(double u, double v, const PixelCoord& p) const = 0;
    virtual T operator()(double u, double v, const PixelCoord& p) const
        {return getValue(u, v,p);}
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

//
// Some simple samplers
// Return solid color
class SolidColorSampler : public ISampleRGBA<PixelRGBA>
{
    PixelRGBA fColor;

public:
    SolidColorSampler(PixelRGBA c) :fColor(c) {}

    virtual PixelRGBA getValue(double u, const PixelCoord& p) const { return fColor; }
    virtual PixelRGBA getValue(double u, double v, const PixelCoord& p) const { return fColor; }
    virtual PixelRGBA getValue(double u, double v, double w, const PixelCoord& p) const { return fColor; }
};
/*
class SolidColorSampler2D : public ISample2D<PixelRGBA>
{
    PixelRGBA fColor;

public:
    SolidColorSampler2D(PixelRGBA c) :fColor(c) {}

    virtual PixelRGBA getValue(double u, double v) const {return fColor;}
};
*/

// grayscale sampler
class GrayscaleSampler : public ISample2D<PixelRGBA>
{
    std::shared_ptr<ISample2D<PixelRGBA> > fWrapped = nullptr;

    static inline uint8_t toGray(const PixelRGBA& pix)
    {
        return (0.2125 * pix.red) + (0.7154 * pix.green) + (0.0721 * pix.blue);
    }

public:
    GrayscaleSampler(std::shared_ptr<ISample2D<PixelRGBA> > wrapped)
        :fWrapped(wrapped)
    {

    }

    PixelRGBA getValue(double u, double v, const PixelCoord &p) const
    {
        // get value from our wrapped sampler
        PixelRGBA c = fWrapped->getValue(u, v, p);

        // convert to grayscale, preserving alpha
        uint8_t g = toGray(c);
        c.red = g;
        c.green = g;
        c.blue = g;

        // return it
        return c;
    }
};

class RainbowSampler : public ISample1D<PixelRGBA>
{
    double fGamma;

public:
    RainbowSampler()
        :fGamma(1.0)
    {}

    RainbowSampler(double gamma)
        :fGamma(gamma)
    {}

    PixelRGBA getValue(double u, const PixelCoord& p) const
    {
        double wl = maths::Map(u, 0, 1, 380, 780);
        auto c = ColorRGBAFromWavelength(wl, fGamma);
        return (PixelRGBA)c;
    }
};
