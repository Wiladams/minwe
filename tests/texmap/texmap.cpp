#include "gui.h"
#include "maths.hpp"

using namespace maths;

// RGBA_Doubles
// 380 <= wl <= 780 nanometers
//
ColorRgba  ColorRGBAFromWavelength(double wl, double gamma = 1.0)
{
    ColorRgba t = { 0,0,0,1 };

    if (wl >= 380.0 && wl <= 440.0) {
        t.red = -1.0f * ((float)wl - 440.0f) / (440.0f - 380.0f);
        t.blue = 1.0;
    }
    else if (wl >= 440.0 && wl <= 490.0) {
        t.green = ((float)wl - 440.0f) / (490.0f - 440.0f);
        t.blue = 1.0;
    }
    else if (wl >= 490.0 && wl <= 510.0) {
        t.green = 1.0;
        t.blue = -1.0f * ((float)wl - 510.0f) / (510.0f - 490.0f);
    }
    else if ((wl >= 510.0) && (wl <= 580.0)) {
        t.red = (float)((wl - 510.0) / (580.0 - 510.0));
        t.green = 1.0;
    }
    else if (wl >= 580.0 && wl <= 645.0) {
        t.red = 1.0;
        t.green = (float)(-1.0 * (wl - 645.0) / (645.0 - 580.0));
    }
    else if (wl >= 645.0 && wl <= 780.0) {
        t.red = 1.0;
    }

    auto s = 1.0;
    if (wl > 700.0) {
        s = 0.3 + 0.7 * (780.0 - wl) / (780.0 - 700.0);
    }
    else if (wl < 420.0) {
        s = 0.3 + 0.7 * (wl - 380.0) / (420.0 - 380.0);
    }

    t.red = (float)pow(t.red * s, gamma);
    t.green = (float)pow(t.green * s, gamma);
    t.blue = (float)pow(t.blue * s, gamma);

    return t;
}


void setup()
{
	setCanvasSize(800, 800);

    for (int y = 0; y < canvasHeight; y++) {
        double gamma = Map(y, 0, canvasHeight, 0.2, 2.0);
        for (int x = 0; x < canvasWidth; x++) {
            auto wl = Map(x, 0, canvasWidth, 380, 780);
            PixelRGBA c = ColorRGBAFromWavelength(wl, gamma);
            gAppSurface->set(x, y, c);
        }
    }
}