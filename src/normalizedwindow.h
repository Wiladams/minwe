#pragma once

#include "pixeltypes.h"


#include <vector>
#include <memory>

// 
// Trying to get the math right on Sub-samples
// We want a SubSample to be able to grab a portion of
// a wrapped sampler, and display it using the range
// [0..1]
//
// Also here is the WindowSampler.  This is a window manager
// where the SubSample is the "Window", and everything is done
// in parametric space [0..1]
//
// A SubSample is just a sampler that wraps
// another sampler.  Since it is a sampler itself
// it maps the range of [0..1] to span the wrapped
// range.  This is good for sprite sheets for example.
// You can have a bunch of graphics on a pixelmap, then 
// just create SubSample objects to present them for rendering
// This way we don't have to copy the graphics into individual
// pixelmaps of their own.  But, the real benefit is you 
// can remain in the normalized coordinates domain.
struct NormalizedWindow : public ISample2D<PixelRGBA, PixelCoord>
{
	std::shared_ptr< ISample2D<PixelRGBA, PixelCoord> > fWrapped;	// The thing we're sub-sampling from
	TexelRect fStickyBounds;
	TexelRect fMovingFrame;
	double uFactor = 1;
	double vFactor = 1;

	NormalizedWindow(const TexelRect& bounds, std::shared_ptr< ISample2D<PixelRGBA, PixelCoord> > wrapped)
		: fWrapped(wrapped)
		, fStickyBounds(bounds)
	{
		setFrame(TexelRect(0, 0, 1, 1));
	}

	NormalizedWindow(const TexelRect& bounds, const TexelRect& frame, std::shared_ptr< ISample2D<PixelRGBA, PixelCoord> > wrapped)
		: fWrapped(wrapped)
		, fStickyBounds(bounds)
	{
		setFrame(frame);
	}

/*
	void moveTo(double u, double v)
	{
		fMovingBounds.moveTo(u, v);
		calcFactors();
	}
*/
	void setFrame(const TexelRect& frame)
	{
		fMovingFrame = frame;
		calcFactors();
	}

	void calcFactors()
	{
		uFactor = fStickyBounds.du() / fMovingFrame.du();
		vFactor = fStickyBounds.dv() / fMovingFrame.dv();
	}

	bool contains(double u, double v)
	{
		return fMovingFrame.contains(u, v);
	}

	PixelRGBA getValue(double u, double v, const PixelCoord& p) override
	{

		double u1 = maths::Map(u, fMovingFrame.left, fMovingFrame.right, fStickyBounds.left,fStickyBounds.right);
		double v1 = maths::Map(v, fMovingFrame.top, fMovingFrame.bottom,fStickyBounds.top,fStickyBounds.bottom);

		double mu = u1;
		double mv = v1;
		return fWrapped->getValue(mu, mv, p);
	}

};

// This is a window manager done in normalized coordinate space
// all the children must be SubSample, which is like a base
// class for windows
struct NormalizedWindowManager : public ISample2D<PixelRGBA, PixelCoord>
{
	std::vector<std::shared_ptr<NormalizedWindow > > fChildren;

	void addChild(std::shared_ptr<NormalizedWindow > aChild)
	{
		fChildren.push_back(aChild);
	}

	// Reference: http://www.guyrutenberg.com/2007/11/19/c-goes-to-operator/
	// useful for while loops
	PixelRGBA getValue(double u, double v, const PixelCoord& p) override
	{
		// find which most visible child the uv coordinates hit
		// ask that child for a value.
		// WAA - an even better thing would be to blend the colors of 
		// all the children below, but the samplers themselves can 
		// do that, so we just grab the highest sampler
		int count = fChildren.size();
		while (count-- > 0)
		{
			if (fChildren[count]->contains(u, v))
				return fChildren[count]->getValue(u, v, p);
		}

		return PixelRGBA(0x0);
	}
};
