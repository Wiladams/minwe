#pragma once

#include "glsl.h"
#include "coloring.h"

#include <array>

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



//
// GraySampler
//
// Converts rgb to a single luminance value.  This goes
// pretty quick as it uses pre-computed lookup tables
// so nothing but lookups and additions.
class GraySampler : public ISample2D<PixelRGBA>
{
    NTSCGray fLuminance;
    std::shared_ptr<ISample2D<PixelRGBA> > fWrapped = nullptr;

public:
    GraySampler(std::shared_ptr<ISample2D<PixelRGBA> > wrapped)
        :fWrapped(wrapped)
    {}

    PixelRGBA getValue(double u, double v, const PixelCoord& p) const
    {
        // get value from our wrapped sampler
        PixelRGBA c = fWrapped->getValue(u, v, p);

        // convert to grayscale, preserving alpha
        uint8_t g = fLuminance.toLuminance(c);
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


