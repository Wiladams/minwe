#pragma once

#include "pixelmap.h"
#include "maths.hpp"
#include "visualeffect.h"

#include <memory>

// SamplerSprite


struct ParametricShard
{
	ParametricRect fParamRect;
	PixelRect fFrame;
	std::shared_ptr<ISample2D<PixelRGBA, PixelCoord> > fWrapped;

	ParametricShard(std::shared_ptr<ISample2D<PixelRGBA, PixelCoord> > wrapped, const PixelRect& origin, const PixelRect &constraint)
		:fWrapped(wrapped),
		fParamRect(origin, constraint)
	{
		fFrame = origin;
	}

	void setFrame(const PixelRect& dstFrame)
	{
		fFrame = dstFrame;
	}

	void draw(std::shared_ptr<PixelMap> pmap)
	{
		// find the intersection between the source rectangle
		// and the frame.  This intersection is the only portion
		// we need to draw, so we know all pixels are in bounds
		PixelRect constraint = pmap->getBounds();
		PixelRect isect = constraint.intersection(fFrame);

		// If there's no intersection with the boundary of the
		// PixelMap, then there's nothing to draw
		if (isect.isEmpty())
			return;

		// Figure out texture boundary based on intersection rectangle
		double left = fParamRect.fTexFrame.left;
		double top = fParamRect.fTexFrame.top;
		double right = fParamRect.fTexFrame.right;
		double bottom = fParamRect.fTexFrame.bottom;

		// Draw the intersected rectangle
		for (int row = isect.y; row < isect.y + isect.height; row++)
		{
			for (int col = isect.x; col < isect.x + isect.width; col++)
			{
				double u = maths::Map(col, isect.x, isect.x + isect.width - 1, left, right);
				double v = maths::Map(row, isect.y, isect.y + isect.height - 1, top, bottom);
				auto c = fWrapped->getValue(u, v, { col,row });
				pmap->set(col, row, c);
			}
		}
	}
};