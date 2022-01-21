#pragma once

#include "visualeffect.h"



// Barn doors do an open reveal
class BarnDoors : public VisualEffect
{
protected:
	StickyWindow fDoor1;	// Left door
	StickyWindow fDoor2;	// Right door

	BarnDoors(double duration, const PixelRect& constraint,
		std::shared_ptr<ISample2D<PixelRGBA, PixelCoord> > s1,
		std::shared_ptr<ISample2D<PixelRGBA, PixelCoord> > s2)
		: VisualEffect(duration, constraint, s1, s2)
	{}

	void setDoor1(const StickyWindow &door) 
	{
		fDoor1 = door;
	}
	
	void setDoor2(const StickyWindow &door) {
		fDoor2 = door;
	}

	PixelRGBA getValue(double u, double v, const PixelCoord& p)
	{
		if (fDoor1.contains(p.x(), p.y()))
			return fDoor1.getValue(u, v, p);

		if (fDoor2.contains(p.x(), p.y()))
			return fDoor2.getValue(u, v, p);

		return fSource2->getValue(u, v, p);
	}
};

class BarnDoorOpenEffect : public BarnDoors
{

public:
	BarnDoorOpenEffect(double duration, const PixelRect& constraint,
		std::shared_ptr<ISample2D<PixelRGBA, PixelCoord> > s1,
		std::shared_ptr<ISample2D<PixelRGBA, PixelCoord> > s2)
		: BarnDoors(duration, constraint, s1, s2)
	{
		// Create two sticky windows 
		// one for the left half
		TexelRect stickyTex1 = TexelRect(0, 0, 0.4999, 1);
		setDoor1(StickyWindow(PixelRect(constraint.x,constraint.y,constraint.width/2,constraint.height), stickyTex1, s1));
		//setDoor1(StickyWindow(PixelRect(constraint.x, constraint.y, constraint.width / 2, constraint.height), constraint, s1));

		// one for the right half
		TexelRect stickyTex2 = TexelRect(0.5, 0, 1, 1);
		setDoor2(StickyWindow(PixelRect(constraint.x+(constraint.width / 2), constraint.y, constraint.width / 2, constraint.height), stickyTex2, s1));
		//setDoor2(StickyWindow(PixelRect(constraint.x + (constraint.width / 2), constraint.y, constraint.width / 2, constraint.height), constraint, s1));
	}

	void onProgress() noexcept
	{
		// adjust the position of the barn doors
		double door1x = maths::Map(progress(), 0, 1, fConstraint.x, fConstraint.x -(fConstraint.width / 2));
		double door1y = 0;
		fDoor1.moveTo(door1x, door1y);

		double door2x = maths::Map(progress(), 0, 1, (fConstraint.width / 2), fConstraint.width);
		double door2y = 0;
		fDoor2.moveTo(door2x, door2y);
	}

};

class BarnDoorCloseEffect : public BarnDoors
{

public:
	BarnDoorCloseEffect(double duration, const PixelRect& constraint,
		std::shared_ptr<ISample2D<PixelRGBA, PixelCoord> > s1,
		std::shared_ptr<ISample2D<PixelRGBA, PixelCoord> > s2)
		: BarnDoors(duration, constraint, s1, s2)
	{
		// Create two sprites, 
		// one for the left half
		setDoor1(StickyWindow(PixelRect(constraint.x, constraint.y, constraint.width / 2, constraint.height), constraint, s1));

		// one for the right half
		setDoor2(StickyWindow(PixelRect(constraint.x + constraint.width / 2, constraint.y, constraint.width / 2, constraint.height), constraint, s1));
	}

	void onProgress() noexcept
	{
		// adjust the position of the barn doors
		double door1x = maths::Map(progress(), 0, 1, fConstraint.x - (fConstraint.width / 2), fConstraint.x);
		double door1y = 0;
		fDoor1.moveTo(door1x, door1y);

		double door2x = maths::Map(progress(), 0, 1, fConstraint.width, (fConstraint.width / 2));
		double door2y = 0;
		fDoor2.moveTo(door2x, door2y);
	}

};
