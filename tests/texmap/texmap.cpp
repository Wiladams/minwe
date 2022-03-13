#include "gui.h"
#include "sampler.h"

void setup()
{
	setCanvasSize(1024, 768);

    for (int y = 0; y < canvasHeight; y++) {
        double gamma = maths::Map(y, 0, canvasHeight, 0.2, 2.0);
        RainbowSampler s(gamma); 
        for (int x = 0; x < canvasWidth; x++) {
            double u = maths::Map(x, 0, canvasWidth - 1, 0, 1);
            auto c = s.getValue(u, {});
            gAppSurface->set(x, y, c);
        }
    }
}