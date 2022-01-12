#include "gui.h"
#include "rendercontext.h"
#include "screensnapshot.h"
#include "maths.hpp"

#include <array>


constexpr int captureWidth = 1920;
constexpr int captureHeight = 1080;

std::shared_ptr<RenderContext> ctx = nullptr;
std::shared_ptr<ScreenSnapshot> screenSamp = nullptr;
std::shared_ptr<GraySampler> gSamp = nullptr;

// Rec 601-1 specification for YCrCb separation
class YCrCbSeparator
{
	NTSCGray grayScaler;

	void separate(ISample2D<PixelRGBA>& src, PixelMap& Y, PixelMap& Cr, PixelMap& Cb)
	{
		// run through the y pixelmap, grabbing items from the src
		// separate along the way
		const auto srcBounds = Y.getBounds();

		for (int row = srcBounds.y; row < srcBounds.y + srcBounds.height - 1; row++)
		{
			for (int col = srcBounds.x; col < srcBounds.x + srcBounds.width - 1; col++) {
				double u = maths::Map(col, srcBounds.x, (double)srcBounds.x + srcBounds.width - 2, 0, 1);
				double v = maths::Map(row, srcBounds.y, static_cast<double>(srcBounds.y + srcBounds.height - 2), 0, 1);

				auto c = src.getValue(u, v, { col,row });

				// dot(color, graycoefficients)
				float Y = (0.299 * c.r()) + (0.587 * c.g()) + (0.114 * c.b());
				float Cr = (0.500 * c.r()) - (0.419 * c.g()) - (0.081 * c.b());
				float Cb = (-0.169 * c.r()) - (0.331 * c.g()) + (0.500 * c.b());
			}
		}
	}
};

void onFrame()
{
    if (screenSamp == nullptr)
        return;

    screenSamp->next();

    //ctx->rectangle(0, 0, canvasWidth, canvasHeight, *gSamp);
    ctx->rect({ 0, 0, (double)canvasWidth/2, (double)canvasHeight }, *screenSamp, { 0,0,0.5,1 });
    ctx->rect({ (double)canvasWidth/2, 0, (double)canvasWidth/2, (double)canvasHeight }, *gSamp, { 0.5,0,1,1 });
}

void setup()
{
	setCanvasSize(captureWidth, captureHeight);
    setFrameRate(15);

    // A rendering context for convenience
    ctx = std::make_shared<RenderContext>(gAppSurface);

	// setup the snapshot
	screenSamp = std::make_shared<ScreenSnapshot>(0, 0, captureWidth, captureHeight);

    // setup the grayscale sampler
    gSamp = std::make_shared<GraySampler>(screenSamp);
}