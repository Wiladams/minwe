#include "gui.h"
#include "sampler.h"

// 
// Be careful with operator precedence
// canvasWidth / freq * 2 !== canvasWidth / (freq*2)
//
void setup()
{
	setCanvasSize(1920, 1920);

    int freq = 64;
    int xExtent = canvasWidth / (freq*2);
    int yExtent = canvasHeight / (freq * 2);
    CheckerPattern chk(0xff555555, 0xffdddddd, freq);


    for (int row=1;row<=freq*2;row++)
        for (int col = 1; col <= freq * 2; col++)
        {
            double u = maths::Map(col, 1, freq*2.0, 0, 1);
            double v = maths::Map(row, 1, freq*2.0, 0, 1);

            auto c = chk(u, v);

            fillRectangle(*gAppSurface, (col-1) * (xExtent), (row-1) * yExtent, xExtent, yExtent, c);
        }
}