#include "gui.h"
#include "stopwatch.h"

using namespace maths;

struct pulseCircle
{
    StopWatch sw;
    double period;

    pulseCircle(const double p = 0.25)
        :period(p)
    {
        sw.reset();
    }

    void draw()
    {
        double t = sw.seconds();
        if (t > period) {
            sw.reset();
            t = 0.0;
        }

        double x = maths::Map(t, 0, period, 0, maths::PiOver2);
        double sinx = sin(x);
        double s = maths::Map(sinx, 0, 1, 1, 5);

        // Calculate the alpha as the inverse of the size
        uint8_t a = maths::Map(s, 1, 5, 255, 50);
        PixelRGBA c = { 255,0,0,a };
        fillEllipse(*gAppSurface, mouseX, mouseY, 20 * s, 20 * s, c);
    }
};

pulseCircle pc1(0.50);
//pulseCircle pc2(0.25);



inline PixelRGBA randomColor(int alpha = 255)
{
    return { (int)maths::random(255), (int)maths::random(255), (int)maths::random(255), alpha };
}

void drawLines()
{
    for (int i = 1; i <= 500; i++)
    {
        int x = maths::Map(mouseX, 0, canvasWidth, (canvasWidth / 2) - 200, (canvasWidth / 2) + 200);
        PixelRGBA c = randomColor();
        line(*gAppSurface, mouseX, mouseY, random(canvasWidth), random(canvasHeight),c);
    }
}




void setup()
{
    setCanvasSize(400, 400);
    setFrameRate(20);
    layered();
}

void onFrame()
{
    background(0xff7f7f7f);

    drawLines();
    pc1.draw();
    //pc2.draw();
}
