#pragma once


#include "animator.h"
#include <memory>

//
// Horizontal fingers
// Do a cover zipper coming from left and right
// Specify how many fingers
//
INLINE std::shared_ptr<WindowAnimation> createHFingersIn(double duration, int howMany,
	std::shared_ptr<ISample2D<PixelRGBA, PixelCoord> > s1,
	std::shared_ptr<ISample2D<PixelRGBA, PixelCoord> > s2)
{
	auto res = std::make_shared<WindowAnimation>(duration);

	auto backing = std::make_shared<SampledWindow>(s1, TexelRect(0, 0, 1, 1));

	// Add the background that is to be covered
	res->addChild(backing);

	// Create the set of fingers which will do the covering
	bool fromTheLeft = true;
	double uSize = 1.0;
	double vSize = 1.0 / howMany;

	for (int i = 0; i < howMany; i++)
	{
		TexelRect beginPos;
		TexelRect endPos(0, i * vSize, 1, (i * vSize) + vSize);

		if (fromTheLeft) {
			beginPos = TexelRect(-1.0, i * vSize, 0.0, (i * vSize) + vSize);
		}
		else {
			beginPos = TexelRect(1.0, i * vSize, 2.0, (i * vSize) + vSize);
		}
		auto finger = std::make_shared<SampledWindow>(s2, endPos);
		auto motion = std::make_shared<TexelRectMotion>(finger->fMovingFrame, beginPos, endPos);

		res->addChild(finger);
		res->addMotion(motion);

		fromTheLeft = !fromTheLeft;
	}

	return res;
}

//
// Vertical Fingers
// do a cover zipper from top and bottom
//
INLINE std::shared_ptr<WindowAnimation> createVFingersIn(double duration, int howMany,
	std::shared_ptr<ISample2D<PixelRGBA, PixelCoord> > s1,
	std::shared_ptr<ISample2D<PixelRGBA, PixelCoord> > s2)
{
	auto res = std::make_shared<WindowAnimation>(duration);

	auto backing = std::make_shared<SampledWindow>(s1, TexelRect(0, 0, 1, 1));

	// Add the background that is to be covered
	res->addChild(backing);

	// Create the set of fingers which will do the covering
	bool fromThisSide = true;
	double uSize = 1.0 / howMany;
	double vSize = 1.0;

	for (int i = 0; i < howMany; i++)
	{
		TexelRect beginPos;
		TexelRect endPos(i * uSize, 0, (i * uSize) + uSize, 1);

		if (fromThisSide) {
			beginPos = TexelRect(i * uSize, -1.0, (i * uSize) + uSize, 0.0);
		}
		else {
			beginPos = TexelRect(i * uSize, 1.0, (i * uSize) + uSize, 2.0);
		}
		auto finger = std::make_shared<SampledWindow>(s2, endPos);
		auto motion = std::make_shared<TexelRectMotion>(finger->fMovingFrame, beginPos, endPos);

		res->addChild(finger);
		res->addMotion(motion);

		fromThisSide = !fromThisSide;
	}

	return res;
}
