#pragma once

#include "pixeltypes.h"


#include <vector>
#include <memory>

struct SubSample : public ISample2D<PixelRGBA, PixelCoord>
{
protected:
	double uFactor = 1;
	double vFactor = 1;

public:
	std::shared_ptr< ISample2D<PixelRGBA, PixelCoord> > fWrapped;	// The thing we're sub-sampling from
	TexelRect fStickyBounds;
	TexelRect fMovingFrame;

	SubSample() { setFrame(TexelRect(0, 0, 1, 1)); }
	SubSample(std::shared_ptr< ISample2D<PixelRGBA, PixelCoord> > wrapped, const TexelRect &bounds)
		:SubSample(wrapped, bounds, bounds) 
	{
		setFrame(TexelRect(0, 0, 1, 1));
	}

	SubSample(std::shared_ptr< ISample2D<PixelRGBA, PixelCoord> > wrapped, 
		const TexelRect &bounds,
		const TexelRect &frame)
		:fWrapped(wrapped)
		,fStickyBounds(bounds)
	{
		setFrame(frame);
	}

	virtual ~SubSample() = default;

	void setBounds(const TexelRect& bounds)
	{
		fStickyBounds = bounds;
		onFrameChanged();
	}

	void setFrame(const TexelRect& frame)
	{
		fMovingFrame = frame;
		onFrameChanged();
	}

	virtual void onFrameChanged() 
	{
		uFactor = ((fStickyBounds.right - fStickyBounds.left) / (fMovingFrame.right - fMovingFrame.left));
		vFactor = ((fStickyBounds.bottom - fStickyBounds.top) / (fMovingFrame.bottom - fMovingFrame.top));
	}

	virtual bool contains(double u, double v)
	{
		return fMovingFrame.contains(u, v);
	}

	virtual PixelRGBA getValue(double u, double v, const PixelCoord& p)
	{
		if (nullptr != fWrapped)
		{
			double u1 = fStickyBounds.left + (u - fMovingFrame.left) * uFactor;
			double v1 = fStickyBounds.top + (v - fMovingFrame.top) * vFactor;

			return fWrapped->getValue(u1, v1, p);
		}

		// return transparent if we're not wrapping anything
		return PixelRGBA(0x0);
	}
};

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
struct SampledWindow : public ISample2D<PixelRGBA, PixelCoord>
{
private:
	double uFactor = 0;
	double vFactor = 0;

public:
	// These are public so they can be animated
	TexelRect fStickyBounds;
	TexelRect fMovingFrame;
	
	// Background image
	std::shared_ptr<ISample2D<PixelRGBA, PixelCoord> > fBackground = nullptr;

public:
	std::vector<std::shared_ptr<SampledWindow > > fChildren;

	SampledWindow() { setFrame(TexelRect(0, 0, 1, 1)); };
	SampledWindow(std::shared_ptr<ISample2D<PixelRGBA, PixelCoord> > background)
		:fBackground(background) 
	{
		setFrame(TexelRect(0, 0, 1, 1));
	}
	SampledWindow(std::shared_ptr<ISample2D<PixelRGBA, PixelCoord> > background, const TexelRect &bounds)
	{
		fBackground = std::make_shared<SubSample>(background, bounds);
		setFrame(TexelRect(0, 0, 1, 1));
	}

	virtual ~SampledWindow() {}

	void setBounds(const TexelRect& bounds)
	{
		fStickyBounds = bounds;
		onFrameChanged();
	}

	void setFrame(const TexelRect& frame)
	{
		fMovingFrame = frame;
		onFrameChanged();
	}

	virtual void onFrameChanged() 
	{ 
		uFactor = ((fStickyBounds.right - fStickyBounds.left) / (fMovingFrame.right - fMovingFrame.left));
		vFactor = ((fStickyBounds.bottom - fStickyBounds.top) / (fMovingFrame.bottom - fMovingFrame.top));
	}


	bool contains(double u, double v)
	{
		return fMovingFrame.contains(u, v);
	}



	// Dealing with Window grouping stuff
	void addChild(std::shared_ptr<SampledWindow > aChild)
	{
		fChildren.push_back(aChild);
	}

	void clearChildren()
	{
		// tell each of the children to clear their children
		fChildren.clear();
	}



	// Reference: http://www.guyrutenberg.com/2007/11/19/c-goes-to-operator/
	// useful for while loops
	PixelRGBA getValue(double parentu, double parentv, const PixelCoord& p) override
	{
		// find which most visible child the uv coordinates hit
		// ask that child for a value.
		int count = fChildren.size();
		double myu = (parentu - fMovingFrame.left)/fMovingFrame.du();
		double myv = (parentv - fMovingFrame.top)/fMovingFrame.dv();

		while (count-- > 0)
		{
			if (fChildren[count]->contains(myu, myv))
				return fChildren[count]->getValue(myu, myv, p);
		}

		if (nullptr != fBackground)
			return fBackground->getValue(myu, myv, p);

		return PixelRGBA(0x0);
	}

};
