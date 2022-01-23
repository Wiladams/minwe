#pragma once

#include "stopwatch.h"
#include "pixelmap.h"
#include "checkerboard.h"
#include "visualeffect.h"



class EffectCheckers : public VisualEffect 
{
	CheckerSampler fSampler;

public:
	EffectCheckers(double duration, int freq,
		std::shared_ptr<ISample2D<PixelRGBA, PixelCoord> > s1,
		std::shared_ptr<ISample2D<PixelRGBA, PixelCoord> > s2)
		: VisualEffect(duration)
		,fSampler(freq, s1, s2)
	{}

	void setFrequency(int freq)
	{
		fSampler.setFrequency(freq);
	}
};