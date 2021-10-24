#include "gui.h"

//
// To get a checkerboard, we're essentially running two
// threshold patterns using a sine wave
// We use the sine function because it ranges from -1 to +1
// over the period of 0 to 2*pi
// from 0 - pi it is positive
// from pi - 2pi it is negative
// We can use this fact to select between two colors
//
class CheckerPattern
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
        frequency(4){}
    
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
};



// 
// Be careful with operator precedence
// canvasWidth / freq * 2 !== canvasWidth / (freq*2)
//
void setup()
{
	setCanvasSize(800, 800);

    int freq = 8;
    int xExtent = canvasWidth / (freq*2);
    int yExtent = canvasHeight / (freq * 2);
    //CheckerPattern chk(0xff00ff00, 0xffff0000, freq);
    CheckerPattern chk;

    for (int row=1;row<=freq*2;row++)
        for (int col = 1; col <= freq * 2; col++)
        {
            double u = maths::Map(col, 1, freq*2.0, 0, 1);
            double v = maths::Map(row, 1, freq*2.0, 0, 1);

            auto c = chk.value(u, v);

            fillRectangle(*gAppSurface, (col-1) * (xExtent), (row-1) * yExtent, xExtent, yExtent, c);
        }
}