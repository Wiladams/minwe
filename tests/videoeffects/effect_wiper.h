#pragma once

#include "animator.h"
#include <memory>

INLINE std::shared_ptr<WindowAnimation> createWiper(double duration,
	std::shared_ptr<ISample2D<PixelRGBA, PixelCoord> > s1,
	std::shared_ptr<ISample2D<PixelRGBA, PixelCoord> > s2)
{
	const int maxRows = 4;
	const int maxColumns = 4;

	auto res = std::make_shared<WindowAnimation>(duration);

	auto backing = std::make_shared<SampledWindow>(s1, TexelRect(0, 0, 1, 1));
	
	// Add the background that is to be covered
	res->addChild(backing);

	TexelRect beginPos(0, 0, 0, 1);
	TexelRect endPos(0, 0, 1, 1);

	auto wiper = std::make_shared<SampledWindow>(s2, TexelRect(0,0,1,1));
	auto frameMotion = std::make_shared<TexelRectMotion>(wiper->fMovingFrame, beginPos, endPos);
	auto boundsMotion = std::make_shared<TexelRectMotion>(wiper->fStickyBounds, beginPos, endPos);

	res->addChild(wiper);
	res->addMotion(frameMotion);
	res->addMotion(boundsMotion);

	return res;
}