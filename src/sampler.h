#pragma once

#include "pixeltypes.h"


#include <array>

//
// Some simple samplers
// Return solid color
class SolidColorSampler : public ISampleRGBA<PixelRGBA, PixelCoord>
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
// LumaSampler
//
// Converts rgb to a single luminance value.  This goes
// pretty quick as it uses pre-computed lookup tables
// so nothing but lookups and additions.
class LumaSampler : public ISample2D<PixelRGBA, PixelCoord>
{
    NTSCGray fLuminance;
    std::shared_ptr<ISample2D<PixelRGBA, PixelCoord> > fWrapped = nullptr;

public:
    LumaSampler(std::shared_ptr<ISample2D<PixelRGBA, PixelCoord> > wrapped)
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


// Some routines in here to deal with colors in various
// ways.
// 
// Utility function.  Turn a visible wavelength into 
// an RGB color value
// 380 <= wl <= 780 nanometers
// gamma typically 1.0
static inline PixelRGBA  ColorRGBAFromWavelength(double wl, double gamma = 1.0)
{

    double red = 0;
    double green = 0;
    double blue = 0;
    double alpha = 1;

    if (wl >= 380.0 && wl <= 440.0) {
        red = -1.0f * ((float)wl - 440.0f) / (440.0f - 380.0f);
        blue = 1.0;
    }
    else if (wl >= 440.0 && wl <= 490.0) {
        green = ((float)wl - 440.0f) / (490.0f - 440.0f);
        blue = 1.0;
    }
    else if (wl >= 490.0 && wl <= 510.0) {
        green = 1.0;
        blue = -1.0f * ((float)wl - 510.0f) / (510.0f - 490.0f);
    }
    else if ((wl >= 510.0) && (wl <= 580.0)) {
        red = (float)((wl - 510.0) / (580.0 - 510.0));
        green = 1.0;
    }
    else if (wl >= 580.0 && wl <= 645.0) {
        red = 1.0;
        green = (float)(-1.0 * (wl - 645.0) / (645.0 - 580.0));
    }
    else if (wl >= 645.0 && wl <= 780.0) {
        red = 1.0;
    }

    auto s = 1.0;
    if (wl > 700.0) {
        s = 0.3 + 0.7 * (780.0 - wl) / (780.0 - 700.0);
    }
    else if (wl < 420.0) {
        s = 0.3 + 0.7 * (wl - 380.0) / (420.0 - 380.0);
    }

    if (gamma != 1.0) {
        red = (float)pow(red * s, gamma);
        green = (float)pow(green * s, gamma);
        blue = (float)pow(blue * s, gamma);
    }

    return PixelRGBA(red * 255, green * 255, blue * 255, 255);
}

class RainbowSampler : public ISample1D<PixelRGBA, PixelCoord>
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


