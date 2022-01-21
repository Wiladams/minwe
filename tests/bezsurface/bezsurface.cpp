/*
    Use Cubic Bezier curves to displace the u,v values 
    of a 2D sampler
*/
#include "gui.h"

#include "sampler.h"
#include "sampledraw2d.h"
#include "screensnapshot.h"

constexpr int WINWIDTH = 1920;
constexpr int WINHEIGHT = 1080;

struct CubicBezier
{
    double p1;
    double p2;
    double p3;
    double p4;

    CubicBezier(double pp1, double pp2, double pp3, double pp4)
        :p1(pp1), p2(pp2), p3(pp3), p4(pp4) {}

    INLINE constexpr double eval(double u) const noexcept
    {

        double oneminusu = 1 - u;
        double BEZ03 = oneminusu * oneminusu * oneminusu;	// (1-u)^3
        double BEZ13 = 3 * u * (oneminusu * oneminusu);	    // 3u(1-u)^2
        double BEZ23 = 3 * u * u * oneminusu;				// 3u^2(1-u)
        double BEZ33 = u * u * u;							// u^3

        return BEZ03 * p1 + BEZ13 * p2 + BEZ23 * p3 + BEZ33 * p4;
    }
};

//
// Use a Cubic Bezier curve to distort the parameters 
// of a sampler
class CubicSurfaceSampler : public ISample2D<PixelRGBA, PixelCoord>
{
    CubicBezier ucurve;     // x, y distortion curve
    CubicBezier vcurve;

    std::shared_ptr<ISample2D<PixelRGBA, PixelCoord> > fWrapped = nullptr;

public:
    CubicSurfaceSampler(std::shared_ptr<ISample2D<PixelRGBA, PixelCoord> > wrapped, const CubicBezier& pc1, const CubicBezier& pc2)
        : fWrapped(wrapped), ucurve(pc1), vcurve(pc2)
    {
    }

    PixelRGBA getValue(double u, double v, const PixelCoord& p) override
    {
        double u1 = ucurve.eval(u);
        double v1 = vcurve.eval(v);

        // get value from our wrapped sampler
        PixelRGBA c = fWrapped->getValue(u1, v1, p);

        return c;
    }
};

std::shared_ptr<ScreenSnapshot> screenSamp = nullptr;
std::shared_ptr<CubicSurfaceSampler> curveSamp = nullptr;

CubicBezier curve1(0.0, 0.25, 0.25, 0.0);
CubicBezier curve2(0.0, 0.25, 0.25, 0.0);

void onFrame()
{
    screenSamp->next();
    sampleRectangle(*gAppSurface, PixelRect(0, 0, canvasWidth, canvasHeight), *curveSamp);
}

void setup()
{
    setCanvasSize(WINWIDTH, WINHEIGHT);
    setFrameRate(15);

    screenSamp = std::make_shared<ScreenSnapshot>(0, 0, WINWIDTH,WINHEIGHT);
    curveSamp = std::make_shared<CubicSurfaceSampler>(screenSamp, 
        curve1, curve2);
}