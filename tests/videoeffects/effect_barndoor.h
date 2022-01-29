#pragma once


#include "animator.h"

// Barn doors do an open reveal
class BarnDoors : public WindowAnimation
{
protected:

	std::shared_ptr<SampledWindow> fDoor1;
	std::shared_ptr<SampledWindow> fDoor2;
	std::shared_ptr<SampledWindow> fDestination;
	
	BarnDoors(double duration,
		std::shared_ptr<ISample2D<PixelRGBA, PixelCoord> > s1,
		std::shared_ptr<ISample2D<PixelRGBA, PixelCoord> > s2)
		: WindowAnimation(duration)
	{
		auto door1Bounds = TexelRect(0, 0, 0.4999, 1);
		auto door2Bounds = TexelRect(0.5, 0, 1, 1);
		auto destinationBounds = TexelRect(0, 0, 1, 1);

		fDoor1 = std::make_shared<SampledWindow>(s1, door1Bounds);
		fDoor2 = std::make_shared<SampledWindow>(s1, door2Bounds);
		fDestination = std::make_shared<SampledWindow>(s2, destinationBounds);

		addChild(fDestination);
		addChild(fDoor1);
		addChild(fDoor2);
	}
};

class BarnDoorOpenEffect : public BarnDoors
{

public:
	BarnDoorOpenEffect(double duration,
		std::shared_ptr<ISample2D<PixelRGBA, PixelCoord> > s1,
		std::shared_ptr<ISample2D<PixelRGBA, PixelCoord> > s2)
		: BarnDoors(duration, s1, s2)
	{		
		addMotion(std::make_shared< TexelRectMotion>(fDoor1->fMovingFrame, TexelRect(0, 0, 0.4999, 1), TexelRect(-0.5, 0, 0.0, 1)));
		addMotion(std::make_shared<TexelRectMotion>(fDoor2->fMovingFrame, TexelRect(0.5, 0, 1, 1), TexelRect(1.0, 0, 1.5, 1)));
	}

};

class BarnDoorCloseEffect : public BarnDoors
{

public:
	BarnDoorCloseEffect(double duration, 
		std::shared_ptr<ISample2D<PixelRGBA, PixelCoord> > s1,
		std::shared_ptr<ISample2D<PixelRGBA, PixelCoord> > s2)
		: BarnDoors(duration, s1, s2)
	{
		addMotion(std::make_shared< TexelRectMotion>(fDoor1->fMovingFrame, TexelRect(-0.5, 0, 0, 1), TexelRect(0, 0, 0.4999, 1)));
		addMotion(std::make_shared<TexelRectMotion>(fDoor2->fMovingFrame, TexelRect(1.0, 0, 1.5, 1), TexelRect(0.5, 0, 1, 1)));
	}
};
