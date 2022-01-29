#pragma once

#include "animator.h"

#define lerp255(bg, fg, a) ((uint32_t)div255((fg*a+bg*(255-a))))

class CrossFadeEffect : public WindowAnimation
{
	std::shared_ptr<ISample2D<PixelRGBA, PixelCoord> > fSource1;
	std::shared_ptr<ISample2D<PixelRGBA, PixelCoord> > fSource2;

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
		double duration,
		std::shared_ptr<ISample2D<PixelRGBA, PixelCoord> > s1,
		std::shared_ptr<ISample2D<PixelRGBA, PixelCoord> > s2)
		:WindowAnimation(duration)
		,fSource1(s1)
		,fSource2(s2)
	{
	}

	INLINE std::shared_ptr<ISample2D<PixelRGBA, PixelCoord> >  source1() { return fSource1; }
	INLINE std::shared_ptr<ISample2D<PixelRGBA, PixelCoord> >  source2() { return fSource2; }

	PixelRGBA getValue(double u, double v, const PixelCoord& p) override
	{
		auto c1 = source1()->getValue(u, v, p);
		auto c2 = source2()->getValue(u, v, p);

		return lerp_pixel(progress(), c1, c2);
	}
};