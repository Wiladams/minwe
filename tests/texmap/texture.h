#pragma once
#include "pixelmap.h"
#include <memory>

using std::shared_ptr;
using std::make_shared;

class PixelTexture {
public:

    virtual PixelRGBA pixelValue(double u, double v) const = 0;

};

class SolidColorTexture : public PixelTexture {
private:
    PixelRGBA fColor;

public:
    SolidColorTexture() {}
    SolidColorTexture(PixelRGBA c) : fColor(c) {}
    SolidColorTexture(double red, double green, double blue)
        : fColor(PixelRGBA(red, green, blue)) {}


    virtual PixelRGBA pixelValue(double u, double v) const 
    {
        return fColor;
    }
};

/*
class checker_texture : public PixelTexture {
public:
    checker_texture() {}
    checker_texture(shared_ptr<PixelTexture> t0, shared_ptr<PixelTexture> t1) : even(t0), odd(t1) {}

    virtual PixelRGBA pixelValue(double u, double v, const vec3& p) const {
        auto sines = sin(10 * p.x) * sin(10 * p.y) * sin(10 * p.z);
        if (sines < 0)
            return odd->pixelValue(u, v, p);
        else
            return even->pixelValue(u, v, p);
    }

public:
    shared_ptr<PixelTexture> odd;
    shared_ptr<PixelTexture> even;
};
*/