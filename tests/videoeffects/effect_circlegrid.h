#pragma once

#include "draw2d.h"

#include <vector>

struct GCircle {
	int fcx = 0;
	int fcy = 0;
	int fradius = 0;

	std::vector<PixelSpan> spans;

	GCircle() = default;
	INLINE GCircle(const GCircle& other) noexcept = default;
	INLINE GCircle& operator=(const GCircle& other) noexcept = default;

	// generate edge list for circle
	void reset(int radius)
	{
		spans.clear();
		fradius = radius;

		// Get the bounding box
		auto x1 = fcx - fradius, y1 = fcy - fradius;
		auto  x2 = fcx + fradius, y2 = fcy + fradius;

		// For each scan line
		for (int y = y1; y < y2; y++)
		{
			int left = 0;
			int right = 0;
			int length = 0;
			bool entered = false;

			// probe until we encounter first entry, which 
			// will become the left edge
			for (int x = x1; x < x2; x++)
			{
				auto distX = floor(x - fcx + 0.5);
				auto distY = floor(y - fcy + 0.5);
				auto distance = sqrt(distX * distX + distY * distY);

				if (distance <= fradius) {
					if (!entered) {
						// Just entered
						entered = true;
						left = x;
						length = length + 1;
					}
					else {
						// increase the length of the span
						length = length + 1;
					}
				}
				else if (entered) {
					// now exiting scanline so save the span
					break;
				}
			}
			spans.push_back({ left, y,length });
		}
	}

	GCircle(int radius)
		:fcx(0), fcy(0), fradius(radius)
	{
		reset(radius);
	}

	void draw(PixelMap& pmap, int cx, int cy, const PixelRGBA& c)
	{
		for (auto& span : spans)
		{
			// offset from whatever origin we used originally
			auto xdiff = cx - fcx;
			auto ydiff = cy - fcy;

			// need to clip
			copySpan(pmap, span.x() + xdiff, span.y() + ydiff, span.w(), c);
		}
	}
};

class CircleGrid
{
	int fRadius = 0;
	int fWidth = 0;
	int fHeight = 0;
	int fNumElements = 0;
	GCircle fCircleInstance;

public:
	CircleGrid(int width, int height, int numElements, int radius = 10)
		:fWidth(width),
		fHeight(height),
		fNumElements(numElements)
	{
		setNumElements(numElements);
	}

	void setRadius(int radius)
	{
		fCircleInstance.reset(radius);
	}

	void setNumElements(int elems)
	{
		int rows = sqrt(fNumElements);
		int cols = rows;

		int stemSize = Clamp(fWidth / cols, 2, 10);
		int stemHalf = (stemSize / 2) + 2;

		fCircleInstance.reset(stemHalf);
	}

	void draw(PixelMap& pmap, std::shared_ptr<ISample2D<PixelRGBA, PixelCoord> > sampler)
	{
		int rows = sqrt(fNumElements);
		int cols = rows;

		int stemSize = Clamp(fWidth / cols, 2, 10);
		int stemHalf = stemSize / 2;

		for (int row = 1; row <= rows; row++)
		{
			for (int col = 1; col <= cols; col++)
			{
				int x = Map(col, 1, cols, 10, fWidth - 10);
				int y = Map(row, 1, rows, 10, fHeight - 10);

				double u = Map(x, 0, (double)(fWidth - 1), 0, 1);
				double v = Map(y, 0, (double)(fHeight - 1), 0, 1);

				auto c = sampler->getValue(u, v, PixelCoord({ x,y }));

				fCircleInstance.draw(pmap, x, y, c);
			}
		}
	}
};
