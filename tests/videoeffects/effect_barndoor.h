#pragma once

#include "visualeffect.h"


struct AnimatedWindow : NormalizedWindow
{
	TexelRect fStartingPosition;
	TexelRect fEndingPosition;

	AnimatedWindow(const TexelRect &bounds, const TexelRect& pos1, const TexelRect& pos2,
		std::shared_ptr < ISample2D<PixelRGBA, PixelCoord> > source)
		:NormalizedWindow(bounds, source)
		, fStartingPosition(pos1)
		, fEndingPosition(pos2)
	{
	}

	void setProgress(double u)
	{
		TexelRect newFrame = maths::Lerp(u, fStartingPosition, fEndingPosition);
		setFrame(newFrame);
	}
};

// Barn doors do an open reveal
class BarnDoors : public VisualEffect
{
	std::shared_ptr<ISample2D<PixelRGBA, PixelCoord> > fSource1;
	std::shared_ptr<ISample2D<PixelRGBA, PixelCoord> > fSource2;

protected:

	std::shared_ptr<AnimatedWindow> fDoor1;
	std::shared_ptr<AnimatedWindow> fDoor2;
	std::shared_ptr<AnimatedWindow> fDoor3;
	
	INLINE std::shared_ptr<ISample2D<PixelRGBA, PixelCoord> >  source1() { return fSource1; }
	INLINE std::shared_ptr<ISample2D<PixelRGBA, PixelCoord> >  source2() { return fSource2; }

	BarnDoors(double duration,
		std::shared_ptr<ISample2D<PixelRGBA, PixelCoord> > s1,
		std::shared_ptr<ISample2D<PixelRGBA, PixelCoord> > s2)
		: VisualEffect(duration)
		, fSource1(s1)
		, fSource2(s2)
	{
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
		initDoors();
	}

	void initDoors() 
	{
		fDoor1 = std::make_shared<AnimatedWindow>(TexelRect(0, 0, 0.4999, 1), TexelRect(0, 0, 0.4999, 1), TexelRect(-0.5, 0, 0, 1), source1());
		fDoor2 = std::make_shared<AnimatedWindow>(TexelRect(0.5, 0, 1, 1), TexelRect(0.5, 0, 1, 1), TexelRect(1.0, 0, 1.5, 1), source1());
		fDoor3 = std::make_shared<AnimatedWindow>(TexelRect(0, 0, 1, 1), TexelRect(0, 0, 1, 1), TexelRect(0, 0, 1, 1), source2());
	
		addChild(fDoor3);
		addChild(fDoor1);
		addChild(fDoor2);
	}

	void onProgress() noexcept
	{
		// adjust the position of the barn doors
		fDoor1->setProgress(progress());
		fDoor2->setProgress(progress());
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
		initDoors();
	}

	void initDoors() 
	{
		// Create two sticky windows 
		fDoor1 = std::make_shared<AnimatedWindow>(TexelRect(0, 0, 0.4999, 1), TexelRect(-0.50, 0, 0, 1), TexelRect(0, 0, 0.4999, 1), source1());
		fDoor2 = std::make_shared<AnimatedWindow>(TexelRect(0.5, 0, 1, 1), TexelRect(1, 0, 1.5, 1), TexelRect(0.5, 0, 1, 1), source1());
		fDoor3 = std::make_shared<AnimatedWindow>(TexelRect(0, 0, 1, 1), TexelRect(0, 0, 1, 1), TexelRect(0, 0, 1, 1), source2());

		addChild(fDoor3);
		addChild(fDoor1);
		addChild(fDoor2);
	}

	void onProgress() noexcept
	{
		fDoor1->setProgress(progress());
		fDoor2->setProgress(progress());
	}
};
