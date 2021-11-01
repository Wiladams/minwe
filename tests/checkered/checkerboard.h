#include <memory>

#include "User32PixelMap.h"
#include "draw2d.h"
#include "sampler.h"
#include "maths.hpp"

//
// Commentary
// Using a checkerboard pattern is a common thing to do in 
// graphics development.  It can serve as a background, letting
// you know when drawing is not present (good for alpha blend testing)
// Or it can be used as a common texture to be wrapped around 
// 3D objects.
// Here we have the sampler and a graphic object.  They are 
// separate because the sampler typically responds to
// parametric u,v values (betwee 0 and 1), whereas the graphic
// object is intended to match actual pixel space, so it 
// has pixel dimensions.
//
// The two could be combined, but I think it's cleaner and
// more versatile to keep them separate.
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
    std::shared_ptr<ISample2D<PixelRGBA> > t1;   // First color
    std::shared_ptr<ISample2D<PixelRGBA> > t2;   // Second color
    int frequency;  // How many times the pattern repeats

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
        :t1(std::make_shared< SolidColorSampler2D>(0xff000000)),
        t2(std::make_shared<SolidColorSampler2D>(0xffffffff)),
        frequency(4) {}

    // Constructor taking two colors and frequency
    CheckerPattern(PixelRGBA c1, PixelRGBA c2, int freq) 
        : t1(std::make_shared< SolidColorSampler2D>(c1)),
        t2(std::make_shared< SolidColorSampler2D>(c2)),
        frequency(freq) {}

    CheckerPattern(
        std::shared_ptr<ISample2D<PixelRGBA> > s1, 
        std::shared_ptr<ISample2D<PixelRGBA> > s2, 
        int freq)
        : t1(s1),t2(s2),frequency(freq) 
    {}

    // u and v range from 0 to 1 inclusive
    // We want to turn that range into values
    // based on the frequency and colors given
    // at construction time
    virtual PixelRGBA getValue(double u, double v) const
    {
        double xrad = maths::Map(u, 0, 1, 0, frequency * (2 * maths::Pi));
        double yrad = maths::Map(v, 0, 1, 0, frequency * (2 * maths::Pi));

        // we need values that are either 1 or -1
        // we change sign at 0
        auto stepu = step(sin(xrad), 0, -1, 1);
        auto stepv = step(sin(yrad), 0, -1, 1);

        auto sines = stepu * stepv;

        return sines <= 0 ? t1->getValue(u,v) : t2->getValue(u,v);
    }


};

// The checkerboard is a graphic which uses the 
// CheckerPattern, and surrounds it with the stuff
// necessary for it to draw independently.
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
    
    Checkerboard(PixelRect frame, 
        std::shared_ptr<ISample2D<PixelRGBA> > s1, 
        std::shared_ptr<ISample2D<PixelRGBA> > s2, 
        int freq)
        : fFrame(frame), 
        fPattern(s1, s2, freq), 
        fFrequency(freq)
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
