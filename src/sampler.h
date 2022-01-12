#pragma once

#include "pixeltypes.h"
#include "coloring.h"

#include <array>

//
// Some simple samplers
// Return solid color
class SolidColorSampler : public ISampleRGBA<PixelRGBA>
{
    PixelRGBA fColor;

public:
    SolidColorSampler(uint32_t c) :fColor(c) {}
    SolidColorSampler(const PixelRGBA& c) :fColor(c){}

    virtual PixelRGBA getValue(double u, const PixelCoord& p) { return fColor; }
    virtual PixelRGBA getValue(double u, double v, const PixelCoord& p) { return fColor; }
    virtual PixelRGBA getValue(double u, double v, double w, const PixelCoord& p) { return fColor; }
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

    PixelRGBA getValue(double u, double v, const PixelCoord& p)
    {
        // get value from our wrapped sampler
        PixelRGBA c = fWrapped->getValue(u, v, p);

        // convert to grayscale, preserving alpha
        uint8_t g = fLuminance.toLuminance(c);

        // return it
        return PixelRGBA(g,g,g,c.a());
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

    PixelRGBA getValue(double u, const PixelCoord& p) 
    {
        double wl = maths::Map(u, 0, 1, 380, 780);
        auto c = ColorRGBAFromWavelength(wl, fGamma);
        return c;
    }
};


