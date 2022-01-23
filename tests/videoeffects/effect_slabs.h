#pragma once

#include "draw2d.h"
#include "rendercontext.h"

#include <vector>


// SamplerSprite
// A rectangle which retains texture coordinates
// no matter where it's moved to
// When it draws, it uses a sampler and the texture
// coordinates
// the texture coordinates are immutable
struct SamplerSprite
{
	PixelRect fOriginFrame;
	TexelRect fTexFrame;

	SamplerSprite(const PixelRect& frame, const PixelRect& constraint)
	{
		PixelRect isect = constraint.intersection(frame);
		fOriginFrame = isect;


		// If no intersection, there's no need to figure
		// out texture coordinates
		if (isect.isEmpty())
			return;

		// Figure out texture coordinates based on the intersection
		double left = Map(isect.x, constraint.x, constraint.x + constraint.width - 1, 0, 1);
		double top = Map(isect.y, constraint.y, constraint.y + constraint.height - 1, 0, 1);
		double right = Map(isect.x + isect.width - 1, constraint.x, constraint.x + constraint.width - 1, 0, 1);
		double bottom = Map(isect.y + isect.height - 1, constraint.y, constraint.y + constraint.height - 1, 0, 1);

		fTexFrame = { left,top,right,bottom };
	}

	void draw(PixelMap& pmap, const PixelRect &dstFrame, ISample2D<PixelRGBA, PixelCoord>& sampler)
	{
		//rect(pmap, fCurrentPosition, *sampler, fTexFrame);

		// find the intersection between the source rectangle
		// and the frame.  This intersection is the only portion
		// we need to draw, so we know all pixels are in bounds
		PixelRect constraint = pmap.getBounds();
		PixelRect isect = constraint.intersection(dstFrame);

		// If there's no intersection with the boundary of the
		// PixelMap, then there's nothing to draw
		if (isect.isEmpty())
			return;

		// Figure out texture boundary based on intersection rectangle
		//double left = Map(isect.x, dstFrame.x, dstFrame.x + dstFrame.width - 1, fTexFrame.left, fTexFrame.right);
		//double top = Map(isect.y, dstFrame.y, dstFrame.y + dstFrame.height - 1, fTexFrame.top, fTexFrame.bottom);
		//double right = Map(isect.x, dstFrame.x, dstFrame.x + dstFrame.width - 1, fTexFrame.left, fTexFrame.right);
		//double bottom = Map(isect.y, dstFrame.y, dstFrame.y + dstFrame.height - 1, fTexFrame.top, fTexFrame.bottom);
		double left = fTexFrame.left;
		double top = fTexFrame.top;
		double right = fTexFrame.right;
		double bottom = fTexFrame.bottom;

		// Draw the intersected rectangle
		for (int row = isect.y; row < isect.y + isect.height; row++)
		{
			for (int col = isect.x; col < isect.x + isect.width; col++)
			{
				double u = maths::Map(col, isect.x, isect.x + isect.width - 1, left, right);
				double v = maths::Map(row, isect.y, isect.y + isect.height - 1, top, bottom);
				auto c = sampler.getValue(u, v, PixelCoord({ col,row }));
				pmap.set(col, row, c);
			}
		}
	}
};

// WindowSlab
// A rectangle that has a variable sampling
// frame based on its frame position
struct WindowSlab
{
	PixelRect fStartPosition;
	PixelRect fEndPosition;
	PixelRect fCurrentPosition;

	double fProgress;

	WindowSlab(const PixelRect& starting, const PixelRect &ending)
		: fStartPosition(starting),
		fEndPosition(ending),
		fCurrentPosition(starting),
		fProgress(0)
	{
	}

	void advance(double amount)
	{
		double newProgress = fProgress + amount;
		
		// do a continuous loop
		if (newProgress > 1.0)
			newProgress = 0;

		progress(newProgress);
	}

	void progress(double pro)
	{
		fProgress = Clamp(pro, 0, 1);
		// linear interpolation between starting and end
		// positions
		double x = Lerp(fProgress, fStartPosition.x, fEndPosition.x);
		double y = Lerp(fProgress, fStartPosition.y, fEndPosition.y);
		double w = Lerp(fProgress, fStartPosition.width, fEndPosition.width);
		double h = Lerp(fProgress, fStartPosition.height, fEndPosition.height);

		fCurrentPosition = PixelRect(x, y, w, h);
	}


	void draw(PixelMap& pmap, ISample2D<PixelRGBA, PixelCoord> &sampler)
	{
		//rect(pmap, fCurrentPosition, *sampler, fTexFrame);

		// find the intersection between the source rectangle
		// and the frame.  This intersection is the only portion
		// we need to draw, so we know all pixels are in bounds
		PixelRect constraint = pmap.getBounds();
		PixelRect isect = constraint.intersection(fCurrentPosition);

		// If there's no intersection with the boundary of the
		// PixelMap, then there's nothing to draw
		if (isect.isEmpty())
			return;

		// Figure out texture boundary based on intersection rectangle
		double left = Map(isect.x, constraint.x, constraint.x + constraint.width - 1, 0, 1);
		double top = Map(isect.y, constraint.y, constraint.y + constraint.height - 1, 0, 1);
		double right = Map(isect.x + isect.width - 1, constraint.x, constraint.x + constraint.width - 1, 0, 1);
		double bottom = Map(isect.y + isect.height - 1, constraint.y, constraint.y + constraint.height - 1, 0, 1);

		// Draw the intersected rectangle
		for (int row = isect.y; row < isect.y + isect.height; row++)
		{
			for (int col = isect.x; col < isect.x + isect.width; col++)
			{
				double u = maths::Map(col, isect.x, isect.x+isect.width-1, left, right);
				double v = maths::Map(row, isect.y, isect.y + isect.height - 1, top, bottom);
				auto c = sampler.getValue(u, v, PixelCoord({ col,row }));
				pmap.set(col, row, c);
			}
		}
	}
};

class WindowSlabber
{
	int slabCount;
	int slabWidth=0;
	int slabLength=0;
	double advancement;

	// a set of slabs
	std::vector<WindowSlab> slabs;

public:
	WindowSlabber(int slabCount, const PixelRect &constraint, double rate=30)
		: slabCount(slabCount),
		advancement(1.0/rate)
	{
		slabWidth = constraint.width / slabCount;
		slabLength = constraint.height / 2;

		for (int i = 0; i < slabCount; i++) {
			WindowSlab newSlab({ slabWidth * i,0,slabWidth,slabLength }, { slabWidth * i,constraint.height,slabWidth,slabLength });

			double u = randomRange(0, 1);
			newSlab.progress(u);

			slabs.push_back(newSlab);
		}
	}

	void draw(PixelMap &pmap, ISample2D<PixelRGBA, PixelCoord> &sampler)
	{
		for (auto& slab : slabs)
		{
			slab.advance(advancement);
			slab.draw(pmap, sampler);
		}
	}
};



