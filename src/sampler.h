#pragma once

#include "pixelmap.h"
#include "maths.hpp"

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
	virtual T operator()(double u) const = 0;
};

// A 2 dimentional sampler
template <typename T>
struct ISample2D
{
	virtual T operator()(double u, double v) const = 0;
};

// A 3 dimensional sampler
template <typename T>
struct ISample3D
{
	virtual T operator()(double u, double v, double w) const = 0;
};


//
// To get a checkerboard, we're essentially running two
// threshold patterns using a sine wave
// We use the sine function because it ranges from -1 to +1
// over the period of 0 to 2*pi
// from 0 - pi it is positive
// from pi - 2pi it is negative
// We can use this fact to select between two colors
//
class CheckerPattern : public ISample2D<PixelRGBA>
{
    PixelRGBA t1;
    PixelRGBA t2;
    int frequency;

    static double step(double u, double threshold, double low, double high)
    {
        if (u < threshold)
            return low;

        return high;
    }

public:
    CheckerPattern()
        :t1(0xff000000),
        t2(0xffffffff),
        frequency(4) {}

    CheckerPattern(PixelRGBA t0, PixelRGBA t1, int freq) : t1(t0), t2(t1), frequency(freq) {}

    virtual PixelRGBA value(double u, double v) const
    {
        double xrad = maths::Map(u, 0, 1, 0, frequency * (2 * maths::Pi * 0.99));
        double yrad = maths::Map(v, 0, 1, 0, frequency * (2 * maths::Pi * 0.99));
        auto sinu = sin(xrad);
        auto sinv = sin(yrad);

        auto stepu = step(sinu, 0, -1, 1);
        auto stepv = step(sinv, 0, -1, 1);

        auto sines = stepu * stepv;

        if (sines >= 0)
            return t1;

        return t2;
    }

    virtual PixelRGBA operator()(double u, double v) const
    {
        return value(u, v);
    }

};
