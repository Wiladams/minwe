#pragma once

#include "animator.h"

// Note:  To generalize flyouts, need to calculate vectors to the 
// circumference of a circle
// Reference:
// https://www.varsitytutors.com/hotmath/hotmath_help/topics/shortest-distance-between-a-point-and-a-circle
//

// Barn doors do an open reveal
class CornersFlyOut : public WindowAnimation
{
protected:

public:
	CornersFlyOut(double duration,
		std::shared_ptr<ISample2D<PixelRGBA, PixelCoord> > s1,
		std::shared_ptr<ISample2D<PixelRGBA, PixelCoord> > s2)
		: WindowAnimation(duration)
	{
		auto destinationBounds = TexelRect(0, 0, 1, 1);
		auto door1Bounds = TexelRect(0.0, 0.0, 0.4999, 0.5);
		auto door2Bounds = TexelRect(0.5, 0.0, 1.0, 0.5);
		auto door3Bounds = TexelRect(0.0, 0.5, 0.5, 1.0);
		auto door4Bounds = TexelRect(0.5, 0.5, 1.0, 1.0);

		auto fDoor1 = std::make_shared<SampledWindow>(s1, door1Bounds);
		auto fDoor2 = std::make_shared<SampledWindow>(s1, door2Bounds);
		auto fDoor3 = std::make_shared<SampledWindow>(s1, door3Bounds);
		auto fDoor4 = std::make_shared<SampledWindow>(s1, door4Bounds);

		addMotion(std::make_shared<TexelRectMotion>(fDoor1->fMovingFrame, TexelRect(0.0, 0.0, 0.4999, 0.5), TexelRect(-0.5, -0.5, 0.0, 0.0)));
		addMotion(std::make_shared<TexelRectMotion>(fDoor2->fMovingFrame, TexelRect(0.5, 0.0, 1.0, 0.5), TexelRect(1.0, -0.5, 1.5, 0.0)));
		addMotion(std::make_shared<TexelRectMotion>(fDoor3->fMovingFrame, TexelRect(0.0, 0.5, 0.5, 1.0), TexelRect(-0.5, 1.0, 0.0, 1.5)));
		addMotion(std::make_shared<TexelRectMotion>(fDoor4->fMovingFrame, TexelRect(0.5, 0.5, 1.0, 1.0), TexelRect(1.0, 1.0, 1.5, 1.5)));


		auto fDestination = std::make_shared<SampledWindow>(s2, destinationBounds);

		addChild(fDestination);
		addChild(fDoor1);
		addChild(fDoor2);
		addChild(fDoor3);
		addChild(fDoor4);
	}


};


