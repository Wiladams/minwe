#pragma once

#include "animator.h"
#include <memory>

INLINE std::shared_ptr<WindowAnimation> createRainBlocks(double duration,
	std::shared_ptr<ISample2D<PixelRGBA, PixelCoord> > s1,
	std::shared_ptr<ISample2D<PixelRGBA, PixelCoord> > s2)
{
	const int maxRows = 4;
	const int maxColumns = 4;

	auto res = std::make_shared<WindowAnimation>(duration);

	auto backing = std::make_shared<SampledWindow>(s1, TexelRect(0, 0, 1, 1));


	// Add the background that is to be covered
	res->addChild(backing);

	// create individual blocks
	double uSize = 1.0/maxColumns;
	double vSize = 1.0 / maxRows;
	double dropDuration = 1.0/maxRows;

	double vOffset = 0.0;
	double uOffset = 0.0;

	for (int col = 0; col<maxColumns; col++) 
	{
		vOffset = 0.0;
		for (int row = 0; row < maxRows; row++) {
			TexelRect endPos(uOffset, vOffset, uOffset + uSize, vOffset + vSize);
			//TexelRect beginPos(uOffset, -vSize, uOffset + uSize, 0);
			TexelRect beginPos(0, -vSize, uSize, 0);

			auto block = std::make_shared<SampledWindow>(s2, endPos);
			auto motion = std::make_shared<TexelRectMotion>(block->fMovingFrame, beginPos, endPos);
			double endTime = ((double)(maxRows-row))*dropDuration;
			double startTime = endTime - dropDuration;

			motion->setTiming(startTime, endTime);

			res->addChild(block);
			res->addMotion(motion);
			vOffset += vSize;
		}
		uOffset += uSize;
	}
	return res;
}
