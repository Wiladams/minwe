#pragma once

//
// A push is typically a displacement, the incoming image
// displaces the outgoing one.  There's nothing really special
// about this class other than it pre-calculates one particular
// motion, that of the push from the left side.
// The second constructor takes all the parameters necessary to 
// achieve a push from anywhere, and since the source and
// destinations can be anything, it doesn't even have to be a push


#include "animator.h"

struct Push : public WindowAnimation
{
	// Quick and dirty left to right horizontal push
	//
	Push(double duration, 
		std::shared_ptr<ISample2D<PixelRGBA, PixelCoord> > s1,
		std::shared_ptr<ISample2D<PixelRGBA, PixelCoord> > s2)
		:WindowAnimation(duration)
	{
		auto pusher = std::make_shared<SampledWindow>(s2, TexelRect(0, 0, 1, 1));
		auto pushee = std::make_shared<SampledWindow>(s1, TexelRect(0, 0, 1, 1));


		addMotion(std::make_shared<TexelRectMotion>(pusher->fMovingFrame, TexelRect(-1, 0, 0, 1), TexelRect(0, 0, 1, 1)));
		addMotion(std::make_shared<TexelRectMotion>(pushee->fMovingFrame, TexelRect(0, 0, 1, 1), TexelRect(1, 0, 2, 1)));
		
		addChild(pusher);
		addChild(pushee);

	}

	//
	// It's a lot of parameters
	// but, you can implement any kind of push by specifying
	// all these parameters
	Push(double duration,
		std::shared_ptr<ISample2D<PixelRGBA, PixelCoord> > s1, const TexelRect &s1bounds, const TexelRect &s1starting, const TexelRect &s1ending,
		std::shared_ptr<ISample2D<PixelRGBA, PixelCoord> > s2, const TexelRect &s2bounds, const TexelRect &s2starting, const TexelRect &s2ending)
		:WindowAnimation(duration)
	{
		auto pusher = std::make_shared<SampledWindow>(s2, s2bounds);
		auto pushee = std::make_shared<SampledWindow>(s1, s1bounds);

		addMotion(std::make_shared<TexelRectMotion>(pusher->fMovingFrame, TexelRect(-1, 0, 0, 1), TexelRect(0, 0, 1, 1)));
		addMotion(std::make_shared<TexelRectMotion>(pushee->fMovingFrame, TexelRect(0, 0, 1, 1), TexelRect(1, 0, 2, 1)));

		addChild(pusher);
		addChild(pushee);
	}

};