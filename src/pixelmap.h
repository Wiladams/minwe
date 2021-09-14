#pragma once

// Doing colors using 'Pixel' which is a 32-bit int
// 0xAARRGGBB
typedef uint32_t Pixel;

inline Pixel rgba(int r, int g, int b, int a = 255)
{
    return a << 24 | r << 16 | g << 8 | b;
}

inline Pixel rgb(int r, int g, int b)
{
    return 255 << 24 | r << 16 | g << 8 | b;
}


class PixelMap
{
public:
    virtual void set(const int x, const int y, const Pixel c)=0;
    virtual Pixel get(const int x, const int y) const = 0;
};

