#pragma once

#include "stopwatch.h"
#include "pixelmap.h"
#include "maths.hpp"

#include <memory>

// A rectangle which retains texture coordinates
// no matter where it's moved to
// When it draws, it uses a sampler and the texture
// coordinates
// the texture coordinates are immutable
struct ParametricRect
{
public:
	TexelRect fTexFrame;		// The texture coordinates
	PixelRect fConstrainedFrame;


	ParametricRect() = default;
	INLINE ParametricRect(const ParametricRect& other) noexcept = default;


	// frame - the portion we want to represent
	// constraint - the boundary within which we are located
	//
	// Calculate a clipped parametric frame, suitable for 
	// rendering later
	ParametricRect(const PixelRect& frame, const PixelRect& constraint)
	{
		PixelRect isect = constraint.intersection(frame);

		// If no intersection, there's no need to figure
		// out texture coordinates
		if (isect.isEmpty())
			return;

		// Figure out texture coordinates based on the intersection
		double left = maths::Map(isect.x, constraint.x, constraint.x + constraint.width - 1, 0, 1);
		double top = maths::Map(isect.y, constraint.y, constraint.y + constraint.height - 1, 0, 1);
		double right = maths::Map(isect.x + isect.width - 1, constraint.x, constraint.x + constraint.width - 1, 0, 1);
		double bottom = maths::Map(isect.y + isect.height - 1, constraint.y, constraint.y + constraint.height - 1, 0, 1);

		fTexFrame = TexelRect(left, top, right, bottom);
		fConstrainedFrame = isect;
	}

};

// StickyWindow
//
// This is core function for many effects.  We want to
// capture a portion of a source, not by copying it, but
// by remembering the parametric location.  So, when we
// move around, like a barn door for example, we retain the
// original pixel coordinates, and can replicate them elsewhere.
// This is done through a combination of "fStickyxxx" 
// fields, and translation in the 'getValue()' function.
//
// Since this is a ISample2D, it can participate in many
// other operations freely
class StickyWindow : public ISample2D<PixelRGBA, PixelCoord>
{
	// The sampler that we will source from
	std::shared_ptr<ISample2D<PixelRGBA, PixelCoord> > fWrapped;

	// The frame being moved somewhere else
	// needed to calculate du/vFactor
	// needed for contains()
	PixelRect fMovingFrame;		
	TexelRect fStickyTexel;
								
	// used to adjust u,v in getValue()
	double duFactor = 1;
	double dvFactor = 1;

public:
	StickyWindow() = default;

	StickyWindow(const PixelRect& frame, const PixelRect& constraint, std::shared_ptr<ISample2D<PixelRGBA, PixelCoord> > wrapped)
		:fWrapped(wrapped)
		,fMovingFrame(frame)
	{
		auto visible = constraint.intersection(frame);
		fStickyTexel = TexelRect::create(visible, constraint);

		setVisible(visible);
	}

	StickyWindow(const PixelRect& frame, const TexelRect& sticky, std::shared_ptr<ISample2D<PixelRGBA, PixelCoord> > wrapped)
		:fWrapped(wrapped)
		, fMovingFrame(frame)
		, fStickyTexel(sticky)
	{
		setVisible(frame);
	}

	// call this whenever changing the location
	// of the window
	void setVisible(const PixelRect& other)
	{
		// Calculate these here so we don't
		// have to calculate them in getValue with a Map()
		duFactor = (fStickyTexel.du() / ((double)fMovingFrame.width - 1));
		dvFactor = (fStickyTexel.dv() / ((double)fMovingFrame.height - 1));
	}
	
	INLINE constexpr PixelRect& movingFrame() noexcept { return fMovingFrame; }

	INLINE void moveBy(int x, int y) noexcept {moveTo(fMovingFrame.x + x, fMovingFrame.y + y);}
	
	INLINE void moveTo(int x, int y) noexcept
	{
		fMovingFrame.moveTo(x, y);
		setVisible(fMovingFrame);
	}

	INLINE bool contains(int x, int y) const noexcept {return fMovingFrame.containsPoint(x, y);}

	// getValue()
	// Return a value from the wrapped sampler.
	// We use the pixel coordinates because we need to adjust
	// to our captured pixel frame.  So, this is not a perfect
	// sampler, as it's location dependent, but that's the 
	// nature of this sampler.
	// Otherwise, we'd have to copy pixels from the source
	PixelRGBA getValue(double u, double v, const PixelCoord& p) override 
	{
		// figure out the u within the moving frame
		// translate to where it would be in the original sticky window
		// add, subtract, multiply
		double mu = fStickyTexel.left + (((double)p.x() - fMovingFrame.x) * duFactor);
		double mv = fStickyTexel.top + (((double)p.y() - fMovingFrame.y) * dvFactor);

		return fWrapped->getValue(mu, mv, p);
	}
};

// VisualEffect
// A visual effect is a sampler, with a timer
// and a duration.  It encapsulates things like
// having two sources
// but, a sub-class can do whatever it likes
// as long as it generates a getValue(u,v)
//
// You can use this base class as a 'do nothing' effect
// as it will always return a transparent pixel
//
class VisualEffect : public ISample2D<PixelRGBA, PixelCoord>
{
protected:
	StopWatch fTimer;
	PixelRect fConstraint;
	double fDuration;
	double fProgress;	// Value from [0..1]
	double fStartTime=0;
	double fEndTime=0;

	std::shared_ptr<ISample2D<PixelRGBA, PixelCoord> > fSource1;
	std::shared_ptr<ISample2D<PixelRGBA, PixelCoord> > fSource2;

public:
	VisualEffect(double duration):VisualEffect(duration, PixelRect(), nullptr, nullptr) {}

	VisualEffect(double duration, const PixelRect& constraint,
		std::shared_ptr<ISample2D<PixelRGBA, PixelCoord> > s1,
		std::shared_ptr<ISample2D<PixelRGBA, PixelCoord> > s2)
		:fProgress(0),
		fConstraint(constraint),
		fDuration(duration),
		fSource1(s1),
		fSource2(s2)
	{}

	virtual ~VisualEffect() = default;

	INLINE std::shared_ptr<ISample2D<PixelRGBA, PixelCoord> >  source1() { return fSource1; }
	INLINE std::shared_ptr<ISample2D<PixelRGBA, PixelCoord> >  source2() { return fSource2; }

	// Maybe want to hold sources in an array to make it more generic
	INLINE void setSource(int index, std::shared_ptr<ISample2D<PixelRGBA, PixelCoord> >  source) {}
	INLINE void setSource1(std::shared_ptr<ISample2D<PixelRGBA, PixelCoord> >  source) { fSource1 = source; }
	INLINE void setSource2(std::shared_ptr<ISample2D<PixelRGBA, PixelCoord> >  source) { fSource2 = source; }

	// This will reset the animation to 
	// its starting point.  It just resets
	// the start time and progress
	void start() noexcept
	{
		fStartTime = fTimer.seconds();
		fEndTime = fStartTime + fDuration;

		fProgress = 0;
	}

	// simply return the progress field
	INLINE double constexpr progress() noexcept { return fProgress; }

	// A function to be implemented by sub-classes
	virtual void onProgress() noexcept
	{
	}

	// externally, an animator must call update for 
	// progress to be made.  You need to either call this
	// or set the progress manually.
	virtual void update() noexcept
	{
		auto u = maths::Map(fTimer.seconds(), fStartTime, fEndTime, 0, 1);
		setProgress(u);
	}

	// If not calling update, the animator can call setProgress
	// explicitly.  This is useful when you're 'scrubbing' through
	// a timeline and want to control exactly how far along the effect is
	void setProgress(double u) noexcept 
	{
		fProgress = maths::Clamp(u, 0, 1);
		this->onProgress();
	}

	// By default, just return a transparent pixel.
	// This is where a sub-class will do all its hardest work
	PixelRGBA getValue(double u, double v, const PixelCoord& p) override {return PixelRGBA();}
};