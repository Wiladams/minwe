#include "gui.h"
#include "stopwatch.h"
#include "recorder.h"

using namespace maths;

Recorder reco;  // The recorder

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


inline PixelRGBA randomColor(uint32_t alpha = 255)
{
    return { (uint32_t)maths::random(255), (uint32_t)maths::random(255), (uint32_t)maths::random(255), alpha };
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
    setCanvasSize(800, 800);
    setFrameRate(15);
    layered();

    reco.init(&*gAppSurface);
}

void onFrame()
{
    background(PixelRGBA(0xff7f7f7f));

    drawLines();
    pc1.draw();

    reco.saveFrame();
}

void keyReleased(const KeyboardEvent& e) {
    switch (e.keyCode)
    {
    case VK_ESCAPE:
        halt();
        break;

    case 'R':
    {
        reco.toggleRecording();
    }
    break;
    }
}
