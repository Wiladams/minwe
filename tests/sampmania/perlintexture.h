#pragma once


#include "pixeltypes.h"
#include "perlin.h"

class NoiseSampler : public ISample2D<PixelRGBA, PixelCoord>
{
public:
    NoiseSampler() = default;
    NoiseSampler(double sc) : scale(sc) {}

    PixelRGBA getValue(double u, double v, const PixelCoord& pt) 
    {
        vec3f p({ (float)u, (float)v, 1.0f });
        auto s = scale * p;
        auto c = vec3f({ 1, 1, 1 }) * 0.5 * (1 + std::sin(s.z() + 10 * noise.turb(s)));
        PixelRGBA pix(c.x() * 255, c.y() * 255, c.z() * 255, 255);

        return pix;
    }

public:
    perlin noise;
    double scale;
};


