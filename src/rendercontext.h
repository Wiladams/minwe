#pragma once

#include <memory>

#include "pixelmap.h"
#include "matrix2d.h"
#include "sampler.h"



class RenderContext
{
	// current transformation matrix
	// PixelMap
	std::shared_ptr<PixelMap> fPixelMap;
	RectD fClipRect;
	Matrix2D fTransform;

public:
	RenderContext(std::shared_ptr<PixelMap> pmap)
		:fPixelMap(pmap)
	{
		fTransform = Matrix2D::createIdentity();
		fClipRect = fPixelMap->frame();
	}

	// Actual drawing
	//
	// point is cool because it allows you to easily map a style
	// across the face of a pixel map
	//
	inline void set(int x, int y, const PixelRGBA &c)
	{
		fPixelMap->set(x, y, c);
	}

	//
	// A single point might have some dimension to it
	// and can be used with styles
	//
	inline void point(const double x, const double y, ISample2D<PixelRGBA, PixelCoord>& style)
	{
		const auto &bounds = fPixelMap->frame();

		// apply transform
		double x1, y1;
		fTransform.transformPoint(x, y, x1, y1);

		// Need to check if real location is still within clip region
		// if it's not, do a quick reject
		if (!bounds.containsPoint(x1, y1))
			return;

		double u = maths::Map(x1, bounds.x, (double)bounds.x + bounds.width - 1, 0, 1);
		double v = maths::Map(y1, bounds.y, (double)bounds.y + bounds.height - 1, 0, 1);


		fPixelMap->set(x1, y1, style.getValue(u, v, PixelCoord({ (int)x,(int)y })));
	}

	// Only do span when transform is identity
	inline void span(const double x, const double y, const double width, ISample1D<PixelRGBA, PixelCoord>& style)
	{
		// apply transform
		// check clipRect

		for (int col = x; col < x + width; col++)
		{
			double u = maths::Map(col, x, (double)x + width - 1, 0, 1);
			PixelRGBA c = style.getValue(u, PixelCoord({ col,(int)y }));
			fPixelMap->set(col, y, c);
		}
	}

	inline void hline(PixelMap& pb, int x1, int y1, int w,
		double v, ISample2D<PixelRGBA, PixelCoord>& style)
	{}

	inline void bezier(PixelMap& pmap, const double x1, const double y1, const double x2, const double y2,
		const double x3, const double y3, const double x4, const double y4, int segments, ISample1D<PixelRGBA, PixelCoord>& style)
	{}

	virtual void convexPolygon(PixelMap& pb,
		PixelCoord* verts, const int nverts, int vmin,
		ISample2D<PixelRGBA, PixelCoord>& style)
	{}

	virtual void rect(const RectD &dstRect, const TexelRect& srcExt, ISample2D<PixelRGBA, PixelCoord>& style)
	{
		// find the intersection between the source rectangle
		// and the frame.  This intersection is the only portion
		// we need to draw, so we know all pixels are in bounds
		PixelRect dstisect = fPixelMap->frame().intersection(dstRect);

		// If there's no intersection with the boundary of the
		// PixelMap, then there's nothing to draw
		if (dstisect.isEmpty())
			return;

		// Calculate these invariants outside the loop
		// to speed things up
		double uadv = (srcExt.right - srcExt.left)/(dstisect.width);
		double vadv = (srcExt.bottom - srcExt.top)/(dstisect.height);

		// Initial u and v values
		double u = srcExt.left;
		double v = srcExt.top;

		for (int row = dstisect.y; row < dstisect.y + dstisect.height; row++)
		{
			for (int col = dstisect.x; col < dstisect.x + dstisect.width; col++)
			{
				auto c = style.getValue(u, v, PixelCoord({ col,row }));
				fPixelMap->set(col, row, c);
				//fPixelMap->copyPixel(col, row, c);

				u += uadv;
			}
			u = srcExt.left;
			v += vadv;
		}
	}

	//
	// Draw a rectangle using the sampler
	// Start with the entire normalized range of the sample
	// adjust to the intersection of the destination frame and the drawing boundary
	virtual void rectangle(const PixelRect &dstFrame, ISample2D<PixelRGBA, PixelCoord>& style)
	{
		PixelRect dstisect = fPixelMap->frame().intersection(dstFrame);
		TexelRect trex = TexelRect::create(dstisect, fPixelMap->frame());

		// could we just do this?
		rect(dstisect, trex, style);
	}

	virtual void triangle(const double x1, const double y1,
		const double x2, const double y2,
		const double x3, const double y3,
		ISample2D<PixelRGBA, PixelCoord>& style)
	{}

	virtual void circle(double centerX, double centerY, double radius, ISample2D<PixelRGBA, PixelCoord>& style)
	{
		auto x1 = centerX - radius, y1 = centerY - radius;
		auto  x2 = centerX + radius, y2 = centerY + radius;
		// should check the clip region to optimize

		// The algorithm here is very straight forward
		// we have a bounding rectangle for the circle
		// so we just check each pixel within that rectangle
		// to see if it's inside, or outside the circle.
		// if it's inside, we set the color, otherwise not
		//
		for (int y = y1; y < y2; y++) {
			for (int x = x1; x < x2; x++) {
				auto distX = (x - centerX + 0.5), distY = (y - centerY + 0.5);
				auto distance = sqrt(distX * distX + distY * distY);
				if (distance <= radius) {
					auto u = maths::Map(x, x1, x2, 0, 1);
					auto v = maths::Map(y, y1, y2, 0, 1);
					auto rgb = style.getValue(u, v, PixelCoord({ x,y }));
					fPixelMap->set(x, y, rgb);
				}
			}
		}

	}

};