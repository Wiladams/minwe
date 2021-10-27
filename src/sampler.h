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
    virtual T getValue(double u) const = 0;
	virtual T operator()(double u) const
    {
        return getValue(u);
    }
};

// A 2 dimentional sampler
template <typename T>
struct ISample2D
{
    virtual T getValue(double u, double v) const = 0;
    virtual T operator()(double u, double v) const
        {return getValue(u, v);}
};

// A 3 dimensional sampler
template <typename T>
struct ISample3D
{
    virtual T getValue(double u, double v, double w) const = 0;
    virtual T operator()(double u, double v, double w) const
    {
        return getValue(u, v, w);
    }
};



//
// Some simple samplers
// Return solid color
class SolidColorSampler : public ISample2D<PixelRGBA>
{
    PixelRGBA fColor;

public:
    SolidColorSampler(PixelRGBA c) :fColor(c) {}

    virtual PixelRGBA getValue(double u, double v) const
    {
        return fColor;
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

    PixelRGBA getValue(double u) const
    {
        double wl = maths::Map(u, 0, 1, 380, 780);
        auto c = ColorRGBAFromWavelength(wl, fGamma);
        return (PixelRGBA)c;
    }
};
