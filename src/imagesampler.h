#pragma once

#include "pixelmap.h"
#include "sampler.h"
#include "maths.hpp"

// A parametric representation of a pixel image
// instead of using "get(x,y)", where x and y are the
// absolute pixel offsets, you use "pixelValue(u,v)"
// In this case, 'u' and 'v' range from 0 to 1 inclusive
//
class ImageSampler : public ISample2D<PixelRGBA>
{
	PixelMap & fImage;
	//bool fFlipVertical;

public:

	ImageSampler(PixelMap & img, const bool flipVertical = false)
		: fImage(img)
		//,fFlipVertical(false)
	{
	}

	PixelRGBA getValue(double u, double v, const PixelCoord &p)  const
	{
		int x = int((u * ((double)fImage.width() - 1)) + 0.5);
		int y = int((v * ((double)fImage.height() - 1)) + 0.5);
		return fImage.get(x, y);

		//return fImage.get(p.x, p.y);
	}
};
