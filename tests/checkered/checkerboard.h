#include <memory>

#include "User32PixelMap.h"
#include "draw2d.h"
#include "sampler.h"
#include "maths.hpp"

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

    // simple step function
    // if the value 'u' is below the threshold, then the low
    // value is returned, otherwise the high value is returned
    // pretty simple ternary operation
    static inline double step(double u, double threshold, double low, double high)
    {
        return u < threshold ? low : high;
    }

public:
    // Default constructor, black and white colors
    CheckerPattern()
        :t1(0xff000000),
        t2(0xffffffff),
        frequency(4) {}

    CheckerPattern(PixelRGBA t0, PixelRGBA t1, int freq) : t1(t0), t2(t1), frequency(freq) {}

    // u and v range from 0 to 1 inclusive
    // We want to turn that range into values
    // based on the frequency and colors given
    // at construction time
    virtual PixelRGBA value(double u, double v) const
    {
        double xrad = maths::Map(u, 0, 1, 0, frequency * (2 * maths::Pi));
        double yrad = maths::Map(v, 0, 1, 0, frequency * (2 * maths::Pi));

        // we need values that are either 1 or -1
        // we change sign at 0
        auto stepu = step(sin(xrad), 0, -1, 1);
        auto stepv = step(sin(yrad), 0, -1, 1);

        auto sines = stepu * stepv;

        return sines <= 0 ? t1 : t2;
    }

    virtual PixelRGBA operator()(double u, double v) const
    {
        return value(u, v);
    }

};

// The checkerboard is a graphic which uses the 
// CheckerPattern, and surrounds it with the stuff
// necessary for it to print independently.
class Checkerboard
{
    CheckerPattern fPattern;
    PixelRect fFrame;
    int xExtent;
    int yExtent;
    int fFrequency;

public:
    Checkerboard(PixelRect frame, PixelRGBA c1, PixelRGBA c2, int freq)
        : fFrame(frame), fPattern(c1, c2, freq), fFrequency(freq)
    {
        xExtent = frame.width / (freq * 2);
        yExtent = frame.height / (freq * 2);
    }

    void draw(std::shared_ptr<User32PixelMap> pmap)
    {
        for (int row = 1; row <= fFrequency * 2; row++)
            for (int col = 1; col <= fFrequency * 2; col++)
            {
                double u = maths::Map(col, 1, fFrequency * 2.0, 0, 1);
                double v = maths::Map(row, 1, fFrequency * 2.0, 0, 1);

                auto c = fPattern(u, v);

                fillRectangle(*pmap, fFrame.x + ((col - 1) * (xExtent)), fFrame.y + ((row - 1) * yExtent), xExtent, yExtent, c);
            }
    }
};
