#pragma once

#include "visualeffect.h"

#define lerp255(bg, fg, a) ((uint32_t)div255((fg*a+bg*(255-a))))

class CrossFadeEffect : public VisualEffect
{
	INLINE static PixelRGBA lerp_pixel(double u, const PixelRGBA& bg, const PixelRGBA& fg)
	{
		uint32_t a = u * 255;
		return PixelRGBA(
			lerp255(bg.r(), fg.r(), a),
			lerp255(bg.g(), fg.g(), a),
			lerp255(bg.b(), fg.b(), a), 255);
	}

public:
	CrossFadeEffect(
		double duration, const PixelRect& constraint,
		std::shared_ptr<ISample2D<PixelRGBA> > s1, 
		std::shared_ptr<ISample2D<PixelRGBA> > s2)
		:VisualEffect(duration, constraint, s1, s2)
	{
	}

	PixelRGBA getValue(double u, double v, const PixelCoord& p)
	{
		auto c1 = fSource1->getValue(u, v, p);
		auto c2 = fSource2->getValue(u, v, p);

		return lerp_pixel(progress(), c1, c2);
	}
};