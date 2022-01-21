#pragma once

#include <cmath>


//
// Representation of color using
// float values.  The intention is to do most
// color handling in this format, then convert to
// specific pixel layouts when needed.
//
// Component values range from [0..1] inclusive
// The data structure should be 16 bytes in size
struct ColorRGBA
{
    float fR;
    float fG;
    float fB;
    float fA;

    INLINE ColorRGBA() = default;
    INLINE ColorRGBA(const ColorRGBA& other) = default;
    INLINE ColorRGBA(float r, float g, float b) noexcept : fR(r), fG(g), fB(b), fA(1.0) {}
    INLINE ColorRGBA(float r, float g, float b, float a) noexcept : fR(r), fG(g), fB(b), fA(a) {}

    // copy constructor
    INLINE ColorRGBA& operator=(const ColorRGBA& other) noexcept = default;

    // Getting and setting fields
    INLINE constexpr float r() const noexcept { return fR; }
    INLINE constexpr float g() const noexcept { return fG; }
    INLINE constexpr float b() const noexcept { return fB; }
    INLINE constexpr float a() const noexcept { return fA; }

    INLINE constexpr void setR(float r) noexcept { fR = r; }
    INLINE constexpr void setG(float g) noexcept { fG = g; }
    INLINE constexpr void setB(float b) noexcept { fB = b; }
    INLINE constexpr void setA(float a) noexcept { fA = a; }

    //INLINE constexpr bool isOpaque() const noexcept { return value >= 0xff000000u; }
    INLINE constexpr bool isTransparent() const noexcept { return (0.0f == fA); }

};

