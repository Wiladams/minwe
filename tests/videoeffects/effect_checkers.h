#pragma once

#include "stopwatch.h"
#include "pixelmap.h"
#include "checkerboard.h"

#include <memory>

class EffectCheckers {
	StopWatch fTimer;
	PixelRect fFrame;
	double fDuration;
	double fProgress;	// Value from [0..1]
	CheckerSampler fSampler;

public:
	EffectCheckers(double secs,
		const PixelRect& frame,
		int freq,
		std::shared_ptr<ISample2D<PixelRGBA> > t1,
		std::shared_ptr<ISample2D<PixelRGBA> > t2)
		: fDuration(secs),
		fFrame(frame),
		fProgress(0),
		fSampler(frame, t1, t2, freq)
	{

	}

	void draw(std::shared_ptr<PixelMap> pmap)
	{
		RenderContext fCtx(pmap);

		fCtx.rect(fFrame, { 0,0,1,1 }, fSampler);
	}

	void setFrequency(int freq)
	{
		fSampler.setFrequency(freq);
	}
};