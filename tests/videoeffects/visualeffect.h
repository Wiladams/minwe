#pragma once

#include "stopwatch.h"
#include "pixelmap.h"
#include "maths.hpp"
#include "normalizedwindow.h"

#include <memory>


/*
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
class VisualEffect : public NormalizedWindow
{
protected:


public:
	VisualEffect(double duration)
	{
		setDuration(duration);
	}


	virtual ~VisualEffect() = default;



};
*/