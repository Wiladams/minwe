#pragma once

#include "apidefs.h"


#include "pixeltypes.h"
#include "easing.h"
#include "interpolator.h"
#include "stopwatch.h"
#include "normalizedwindow.h"

#include <functional>
#include <vector>
#include <memory>

struct IAnimateField
{
protected:
	PFunc1Double fEasing = easing::easeLinear;
	double fStartTime = 0.0;
	double fEndTime = 1.0;

public:
	IAnimateField() = default;

	IAnimateField(double startTime, double endTime)
		:fStartTime(startTime),
		fEndTime(endTime)
	{

	}

	virtual ~IAnimateField() {}

	void setTiming(double startTime, double endTime)
	{
		fStartTime = startTime;
		fEndTime = endTime;
	}

	virtual void updateField(double u)
	{
		if ((u < fStartTime) || (u>fEndTime))
			return;

		//u = maths::Clamp(u, 0, 1);
		double u1 = (u - fStartTime)/(fEndTime-fStartTime);

		onUpdateField(u1);
	}

	virtual void onUpdateField(double u) = 0;

	void setEasing(PFunc1Double ease)
	{
		fEasing = ease;
	}
};

//
// Animotion
// This defines the motion associated with an animation
// it is templatized, because an animation can decide to 
// attach motion to any number of attributes within an animation
template <typename T>
struct FieldAnimator : public IAnimateField
{
private:
	T fStartValue;
	T fEndValue;
	T& fField;

protected:


public:

	FieldAnimator(T& field, const T& startValue, const T& endValue)
		: IAnimateField(0.0,1.0)
		,fField(field)
		,fStartValue(startValue)
		,fEndValue(endValue)
	{}

	FieldAnimator(T& field, const T& startValue, const T& endValue, double startTime, double endTime)
		: IAnimateField(startTime, endTime)
		, fField(field)
		, fStartValue(startValue)
		, fEndValue(endValue)
	{}

	// When progress is made, alter the field
	// 
	void onUpdateField(double u) override
	{
		fField = this->operator()(u);
	}

	T operator() (double u)
	{
		double u1 = fEasing(u);
		T newValue = maths::Lerp(u1, fStartValue, fEndValue);
		return newValue;
	}
};

using TexelRectMotion = FieldAnimator<TexelRect>;



//
// WindowAnimation
//
// Animation of a Window.  The actual animation is 
// up to a sub-class.  Any attribute of the window
// can be animated, and interpolators are required
// to implement the intended animation.
//
struct WindowAnimation : public SampledWindow
{
	bool fIsRunning = false;

	// Time associations
	StopWatch fTimer;
	double fDuration = 0;
	double fStartTime = 0;
	double fEndTime = 0;
	double fProgress = 0;	// Value from [0..1]
	std::vector<std::shared_ptr<IAnimateField> > fAnimators;

	WindowAnimation	(double duration)
		:fDuration(duration)
	{}

	virtual void setDuration(double dur) { fDuration = dur; }
	void setEasing(PFunc1Double ease) 
	{
		for (auto& mover : fAnimators) {
			mover->setEasing(ease);
		}
	}


	void addMotion(std::shared_ptr<IAnimateField> motion)
	{
		fAnimators.push_back(motion);
	}

	void addMovingSample(std::shared_ptr<SampledWindow> win, std::shared_ptr<IAnimateField> motion)
	{
		addChild(win);
		addMotion(motion);
	}

// This will reset the animation to 
// its starting point.  It just resets
// the start time and progress
// BUGBUG
// WAA - The problem here is that although 
// we start, progress is governed by calling
// udate(), or setProgress()
// if neither of those occur, we don't actually
// progress
	void start() noexcept
	{
		fStartTime = fTimer.seconds();
		fEndTime = fStartTime + fDuration;

		fProgress = 0;
		fIsRunning = true;
	}

	// simply return the progress field
	INLINE double constexpr progress() noexcept { return fProgress; }

	// If not calling update, the animator can call setProgress
// explicitly.  This is useful when you're 'scrubbing' through
// a timeline and want to control exactly how far along the effect is
	void setProgress(double u)
	{
		fProgress = maths::Clamp(u,0,1);
		onProgress(fProgress);
	}

	// externally, an animator must call update for 
	// progress to be made.  You need to either call this
	// or set the progress manually.
	void update() noexcept
	{
		auto u = maths::Map(fTimer.seconds(), fStartTime, fEndTime, 0, 1);
		fProgress = maths::Clamp(u, 0, 1);

		onProgress(fProgress);
	}



	// A function to be implemented by sub-classes
	// by default, just tell all the children
	// of the progress
	virtual void onProgress(double u)
	{
		for (auto& motion : fAnimators)
		{
			motion->updateField(u);
		}
	}

};



struct StoryLine
{

};