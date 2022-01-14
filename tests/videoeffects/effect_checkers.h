#pragma once

#include "stopwatch.h"
#include "pixelmap.h"
#include "checkerboard.h"
#include "visualeffect.h"



class EffectCheckers : public VisualEffect 
{
	CheckerSampler fSampler;

public:
	EffectCheckers(double duration, const PixelRect& constraint, int freq,
		std::shared_ptr<ISample2D<PixelRGBA> > s1,
		std::shared_ptr<ISample2D<PixelRGBA> > s2)
		: VisualEffect(duration, constraint, s1,s2)
		,fSampler(freq, s1, s2)
	{}

	void setFrequency(int freq)
	{
		fSampler.setFrequency(freq);
	}
};