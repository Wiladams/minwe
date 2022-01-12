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

// Represents an initial portion of a boundary
// As a sampler, it will subsequently offer pixel
// values based on parameters, with origin relative
// to the captured region
class StickyWindow : public ISample2D<PixelRGBA>
{
	std::shared_ptr<ISample2D<PixelRGBA> > fWrapped;

	PixelRect fConstraint;		// The boundary within which we move
	
	PixelRect fStickyFrame;
	PixelRect fStickyVisible;
	TexelRect fStickyTexel;

	PixelRect fMovingFrame;
	PixelRect fVisibleFrame;
	TexelRect fVisibleTexel;

public:
	StickyWindow() = default;

	StickyWindow(const PixelRect& frame, const PixelRect& constraint, std::shared_ptr<ISample2D<PixelRGBA> > wrapped)
		:fWrapped(wrapped),
		fConstraint(constraint),
		fStickyFrame(frame),
		fMovingFrame(frame)
	{
		fStickyVisible = fConstraint.intersection(frame);
		fStickyTexel = TexelRect::create(fStickyVisible, fConstraint);

		setVisible(fStickyVisible);
	}

	void setVisible(const PixelRect& other)
	{
		fVisibleFrame = fConstraint.intersection(other);
		fVisibleTexel = TexelRect::create(fVisibleFrame, fConstraint);
	}
	
	PixelRect& movingFrame() { return fMovingFrame; }

	void moveBy(int x, int y)
	{
		fMovingFrame.x += x;
		fMovingFrame.y += y;

		setVisible(fMovingFrame);
	}
	
	void moveTo(int x, int y)
	{
		fMovingFrame.x = x;
		fMovingFrame.y = y;

		setVisible(fMovingFrame);
	}

	bool contains(int x, int y)
	{
		return fVisibleFrame.containsPoint(x, y);
	}

	PixelRGBA getValue(double u, double v, const PixelCoord& p)
	{
		// figure out the u within the moving frame
		// First translate the point into the frame of the moving window
		int mx = p.x;
		int my = p.y;

		// Then, determine what the u and v values are within the moving frame
		// translate to where it would be in the original sticky window
		double mu = maths::Map(mx, fMovingFrame.x, fMovingFrame.x + fMovingFrame.width - 1, fStickyTexel.left, fStickyTexel.right);
		double mv = maths::Map(my, fMovingFrame.y, fMovingFrame.y + fMovingFrame.height - 1, fStickyTexel.top, fStickyTexel.bottom);

		return fWrapped->getValue(mu, mv, p);
	}
};

// A visual effect is just a sampler, with a timer
// by default, it assumes there are two video sources
// but, a sub-class can do whatever it likes
// as long as it generates a getValue(u,v)
//
// You can use this base class as a 'do nothing' effect
// as it will always return a transparent pixel
//
class VisualEffect : public ISample2D<PixelRGBA>
{
protected:
	StopWatch fTimer;
	PixelRect fConstraint;
	double fDuration;
	double fProgress;	// Value from [0..1]
	double fStartTime=0;
	double fEndTime=0;

	std::shared_ptr<ISample2D<PixelRGBA> > fSource1;
	std::shared_ptr<ISample2D<PixelRGBA> > fSource2;

public:
	VisualEffect(double duration, const PixelRect& constraint,
		std::shared_ptr<ISample2D<PixelRGBA> > s1,
		std::shared_ptr<ISample2D<PixelRGBA> > s2)
		:fProgress(0),
		fConstraint(constraint),
		fDuration(duration),
		fSource1(s1),
		fSource2(s2)
	{}

	virtual ~VisualEffect() {}

	INLINE std::shared_ptr<ISample2D<PixelRGBA> >  source1() { return fSource1; }
	INLINE std::shared_ptr<ISample2D<PixelRGBA> >  source2() { return fSource2; }

	INLINE void setSource1(std::shared_ptr<ISample2D<PixelRGBA> >  source) { fSource1 = source; }
	INLINE void setSource2(std::shared_ptr<ISample2D<PixelRGBA> >  source) { fSource2 = source; }

	void start() noexcept
	{
		fStartTime = fTimer.seconds();
		fEndTime = fStartTime + fDuration;

		fProgress = 0;
	}

	INLINE double constexpr progress() noexcept { return fProgress; }

	virtual void onProgress() noexcept
	{
	}

	virtual void update() noexcept
	{
		auto u = maths::Map(fTimer.seconds(), fStartTime, fEndTime, 0, 1);
		setProgress(u);
	}

	void setProgress(double u) noexcept 
	{
		fProgress = maths::Clamp(u, 0, 1);
		this->onProgress();
	}

	PixelRGBA getValue(double u, double v, const PixelCoord& p){return PixelRGBA();}
};
