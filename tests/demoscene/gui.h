#pragma once

/*
	This single file contains a bunch of stuff necessary to 
	create a typical User Interface environment.  It has 
	a drawing context, which manages stateful drawing
	and it has elements such as graphics, windows, and the
	like.
*/

#include "uievent.h"
#include "maths.hpp"
#include "matrix2d.h"
#include "pixelsurface.h"

#include <memory>
#include <vector>
#include <deque>





class IDrawable
{
protected:
	virtual ~IDrawable() {}

public:

	virtual void draw(std::shared_ptr<IDrawingContext> ctx) = 0;
};

/*
	A Graphic is something that has a preferred
	size, a boundary, and a frame.

	PreferredSize - How big the graphic would like to be
	Bounds - How big the graphic actually is
	Frame - The location, within the bounds of the parent frame

*/
class IGraphic : public IDrawable
{
protected:
	virtual ~IGraphic() {};

public:
	//virtual std::shared_ptr<IDrawingContext> getPreferredRenderer() const { return nullptr; }


	virtual bool contains(int x, int y)
	{
		auto f = getFrame();
		return ((x >= f.x) && (y >= f.y) &&
			(x - f.x <= f.width) &&
			(y - f.y <= f.height));
	}

	virtual PixelRect getFrame() const = 0;
	virtual void moveTo(const double x, const double y) = 0;

	virtual void moveBy(double dx, double dy)
	{
		auto f = getFrame();
		moveTo(f.x + dx, f.y + dy);
	}

	virtual void mouseEvent(const MouseEvent& e) = 0;
	virtual void mouseMoved(const MouseEvent& e) = 0;
	virtual void mouseDragged(const MouseEvent& e) = 0;
	virtual void mousePressed(const MouseEvent& e) = 0;
	virtual void mouseReleased(const MouseEvent& e) = 0;

	// Keyboard events
	//virtual void keyPressed(const KeyboardEvent& e) =0;
	//virtual void keyReleased(const KeyboardEvent& e)=0;
	//virtual void keyTyped(const KeyboardEvent& e)=0;

		// File dropping
	virtual void fileDrop(const FileDropEvent& e) = 0;
};

/*
	given a vector of graphics, perform
	whatever layout operation we need to
*/
class ILayoutGraphics {
public:

	virtual void layout(std::deque<std::shared_ptr<IGraphic> > gs)
	{
		// by default, do nothing
	}
};


class BinaryLayout : ILayoutGraphics
{

	enum class BinaryAlignment : unsigned
	{
		CENTER,
		TOP,
		RIGHT,
		BOTTOM,
		LEFT
	};

	double fGap;
	BinaryAlignment fAlign;

public:

	BinaryLayout(std::shared_ptr<IGraphic> primary,
		std::shared_ptr<IGraphic> secondary,
		BinaryAlignment align,
		double gap)
		: fAlign(align),
		fGap(gap)
	{
		std::deque<std::shared_ptr<IGraphic> > gs{
			primary,
			secondary
		};

		layout(gs);
	}

	BinaryLayout(BinaryAlignment align, double gap)
		:fGap(gap),
		fAlign(align)
	{
	}

	virtual void layout(std::deque<std::shared_ptr<IGraphic> > gs)
	{
		// There must be at least two graphics in the vector
		// only the first two will be part of the layout
		if (gs.size() < 2)
			return;

		auto primary = gs[0];
		auto pFrame = primary->getFrame();
		auto secondary = gs[1];
		auto sFrame = secondary->getFrame();

		switch (fAlign) {
		case BinaryAlignment::CENTER: {
			// use center of primary to be center of secondary
			double pCenterX = pFrame.x + pFrame.width / 2.0;
			double pCenterY = pFrame.y + pFrame.height / 2.0;
			sFrame.x = pCenterX - sFrame.width / 2.0;
			sFrame.y = pCenterY - sFrame.height / 2.0;
		}
									break;

		case BinaryAlignment::TOP:
			break;

		case BinaryAlignment::RIGHT:
			break;

		case BinaryAlignment::BOTTOM:
			break;

		case BinaryAlignment::LEFT:
			break;
		}

		secondary->moveTo(sFrame.x, sFrame.y);
	}
};

class CascadeLayout : public ILayoutGraphics
{
	int wX = 10;
	int wY = 49;
	int fWidth;
	int fHeight;
	int fVerticalOffset = 10;
	int fHorizontalOffset = -10;
	int fVerticalGap = 8;

public:
	CascadeLayout(int w, int h)
	{
		fWidth = w;
		fHeight = h;
	}

	virtual void addWindow(std::shared_ptr<IGraphic> win)
	{
		int x = wX;
		int y = wY;

		wX += (int)(win->getFrame().width + fHorizontalOffset);
		wY += (int)fVerticalOffset;

		if (wX > fWidth - 256)
		{
			wX = 10;
			wY += 256 + fVerticalGap;
		}

		win->moveTo(x, y);
	}

	// Perform layout starting from scratch
	void layout(std::deque<std::shared_ptr<IGraphic> > gs)
	{
		int x = 10;
		int y = 49;

		for (std::shared_ptr<IGraphic> g : gs)
		{
			int offsetx = x;
			int offsety = y;

			x += (int)(g->getFrame().width + fHorizontalOffset);
			y += fVerticalOffset;

			if (x > fWidth - 256) {
				x = 10;
				y += 256 + fVerticalGap;
			}

			g->moveTo(offsetx, offsety);
		}
	}
};

/*
	Base class representing a contained graphic object
	Deals with children, layout, drawing, and interaction
*/
class Graphic : public IGraphic
{
protected:
	std::deque<std::shared_ptr<IGraphic> > fChildren;
	std::shared_ptr<ILayoutGraphics> fLayout;
	std::shared_ptr<IGraphic> fActiveGraphic;



	Matrix2D fTransform;  // Internal transformation matrix
	PixelRect fBounds{};
	PixelRect fFrame{};

public:
	Graphic()
		:Graphic(0.0, 0, 0, 0)
	{}


	Graphic(const double x, const double y, const double w, const double h)
		:fFrame(x, y, w, h),
		fBounds(0, 0, w, h)
	{
		fTransform = Matrix2D::createIdentity();
	}

	Graphic(const PixelRect& frame)
		:fFrame(frame),
		fBounds(0, 0, frame.width, frame.height),
		fActiveGraphic(nullptr)
	{
		fTransform = Matrix2D::createIdentity();
	}


	PixelRect getBounds() const { return fBounds; }

	void setFrame(const PixelRect& frame) { fFrame = frame; }
	virtual PixelRect getFrame() const { return fFrame; }

	void setLayout(std::shared_ptr<ILayoutGraphics> layout)
	{
		fLayout = layout;
	}

	void translateBy(double x, double y)
	{
		fTransform.translate(x, y);
	}

	void setTransform(Matrix2D& m) { fTransform = m; }
	Matrix2D& getTransform() { return fTransform; }

	virtual bool contains(int x, int y)
	{
		return ((x >= fFrame.x) && (y >= fFrame.y) &&
			(x - fFrame.x <= fFrame.width) &&
			(y - fFrame.y <= fFrame.height));
	}

	void addChild(std::shared_ptr<IGraphic> child)
	{
		fChildren.push_back(child);
		if (nullptr != fLayout) {
			fLayout->layout(fChildren);
		}
	}

	// Find the topmost window at a given position
	std::shared_ptr<IGraphic> graphicAt(int x, int y)
	{
		// traverse through windows in reverse order
		// return when one of them contains the mouse point
		std::deque<std::shared_ptr<IGraphic> >::reverse_iterator rit = fChildren.rbegin();
		for (rit = fChildren.rbegin(); rit != fChildren.rend(); ++rit)
		{
			if ((*rit)->contains(x, y))
				return *rit;
		}

		return nullptr;
	}

	virtual void setActiveGraphic(std::shared_ptr<IGraphic> g)
	{
		fActiveGraphic = g;
	}

	void moveToFront(std::shared_ptr<IGraphic> g)
	{
		std::deque<std::shared_ptr<IGraphic> >::iterator it = fChildren.begin();
		for (it = fChildren.begin(); it != fChildren.end(); ++it)
		{
			if (*it == g) {
				fChildren.erase(it);
				fChildren.push_back(g);
				break;
			}
		}
	}

	void moveTo(double x, double y)
	{
		fFrame.x = x;
		fFrame.y = y;
	}

	virtual void drawBackground(std::shared_ptr<IDrawingContext> ctx)
	{
		//ctx->flush();
	}

	virtual void drawChildren(std::shared_ptr<IDrawingContext> ctx)
	{
		for (std::shared_ptr<IGraphic> g : fChildren)
		{
			// BUGBUG - really there shouldn't be any nullptr graphics
			if (nullptr == g)
				continue;

			g->draw(ctx);
		}
	}

	virtual void drawSelf(std::shared_ptr<IDrawingContext> ctx)
	{
		// this one sub-classes should implement
		// if they like
	}

	virtual void drawForeground(std::shared_ptr<IDrawingContext> ctx)
	{
		//ctx->flush();
	}



	virtual void draw(std::shared_ptr<IDrawingContext> ctx)
	{
		ctx->push();
		ctx->clip(fFrame.x, fFrame.y, fFrame.width, fFrame.height);

		// BUGBUG - maybe perform arbitrary transform?
		double x = fFrame.x;
		double y = fFrame.y;
		auto pt = fTransform.transformPoint(x, y);
		ctx->translate(x, y);

		drawBackground(ctx);
		drawChildren(ctx);
		drawSelf(ctx);
		drawForeground(ctx);

		ctx->noClip();
		ctx->pop();

	}


	// Handling mouse events
	virtual void mouseEvent(const MouseEvent& e)
	{
		// translate according to the transformation
		//auto pt = fTransform.mapPoint(e.x, e.y);
		//std::cout << "graphic.mouseEvent original: " << e.x << ", " << e.y << " modified: " << pt.x << ", " << pt.y << std::endl;
		//std::cout << "graphic.mouseEvent original: " << e.x << ", " << e.y << std::endl;

		// Figure out which child the mouse pointer 
		// is currently over
		auto g = graphicAt(e.x, e.y);

		// translate according to the transformation
		//auto pt = fTransform.mapPoint(e.x, e.y);
		//auto newEvent(e);
		//newEvent.x = (int)(pt.x + fFrame.x);
		//newEvent.y = (int)(pt.y + fFrame.y);

		if (g != nullptr) {
			// If it's a sub-graphic, then continue down the chain
			auto newEvent = e;
			newEvent.x = (int)(e.x - g->getFrame().x);
			newEvent.y = (int)(e.y - g->getFrame().y);

			g->mouseEvent(newEvent);
		}
		else {
			// If the mouse event didn't land on a child
			// then do our own processing
			switch (e.activity) {
			case MOUSEDRAGGED:
				mouseDragged(e);
				break;

			case MOUSEPRESSED:
				mousePressed(e);
				break;

			case MOUSEMOVED:
				mouseMoved(e);
				break;

			case MOUSERELEASED:
				mouseReleased(e);
				break;

			case MOUSEWHEEL:
				mouseWheel(e);
				break;
			}
		}

	}

	virtual void mouseMoved(const MouseEvent& e)
	{
		// do nothing
	}

	virtual void mouseDragged(const MouseEvent& e)
	{
		// do nothing
	}

	virtual void mousePressed(const MouseEvent& e)
	{
		// do nothing
		printf("Graphic::mousePressed\n");
	}

	virtual void mouseReleased(const MouseEvent& e)
	{
		// do nothing
	}

	virtual void mouseWheel(const MouseEvent& e)
	{
		// do nothing
	}

	virtual void fileDrop(const FileDropEvent& e)
	{
		// do nothing
	}

};

/*
	The GView has a couple of primary
		purposes.
		1) Be a clipping region for an underlying graphic
		2) Be a drawing transformation for the underlying graphic

		The clipping region is taken care of by simply having a.frame
		property.The higher level(window, or graphicgroup) will automatically
		clip, and translate to this coordinate system.

		Beyond that, the view is essentially a two dimensional slider thumb.
		You can alter it's base transformation using setPosition(posX, posY)

		The position varies between 0 and 1 in both directions.

		The view must know how to map from the position value to the appropriate
		transformation values.This is done based on the size and potential
		location of the view relative to the size and location of the page.
*/

using std::shared_ptr;

class PageView : public Graphic
{
	shared_ptr<IDrawable> fPage;

public:
	PageView(const PixelRect& r)
		:Graphic(r)
	{
	}

	PageView(const double x, const double y, const double w, const double h)
		: Graphic(PixelRect(x, y, w, h))
	{
	}

	shared_ptr<IDrawable> getPage() { return fPage; }
	void setPage(shared_ptr<IDrawable> page) { fPage = page; }

	virtual void drawSelf(std::shared_ptr<IDrawingContext> ctx)
	{
		if (nullptr != fPage) {
			fPage->draw(ctx);
		}
	}
};

class GWindow : public Graphic
{
protected:
	PixelRect fClientArea;
	PixelCoord fLastMouse;
	bool fIsMoving;
	PixelRGBA fBackgroundColor;
	PixelRect fTitleBar;
	PixelRGBA fTitleBarColor;
	std::string fTitle;

	std::shared_ptr<PixelSurface> fSurface;	// backing store for drawing

public:

	GWindow(int x, int y, int w, int h)
		: Graphic(x, y, w, h),
		fClientArea(0, 0, w, h),
		fTitleBar(2, 2, w, 32),
		fTitleBarColor(225, 220, 220, 127),
		fBackgroundColor(245, 246, 247),
		fLastMouse(0, 0),
		fIsMoving(false)
	{
		fSurface = std::make_shared<PixelSurface>(w, h);
	}

	PixelMap& getImage() { return fSurface->getImage(); }

	void setBackgroundColor(const Pixel& c)
	{
		fBackgroundColor = c;
	}

	void drawBackground(std::shared_ptr<IDrawingContext> ctx)
	{
		//std::cout << "GWindow::drawBackground" << std::endl;

		ctx->push();

		// Fill in background
		ctx->noStroke();
		ctx->fill(fBackgroundColor);
		ctx->rect(fClientArea.x, fClientArea.y, fClientArea.width, fClientArea.height);

		// Draw a frame
		auto frame = getFrame();

		ctx->strokeWeight(2.0);
		ctx->stroke(0);
		ctx->noFill();
		ctx->rect(0, 0, frame.width, frame.height);

		ctx->pop();
	}

	void draw(std::shared_ptr<IDrawingContext> ctx)
	{
		fSurface->push();
		//ctx->clip(fFrame.x, fFrame.y, fFrame.w, fFrame.h);


		// BUGBUG - maybe perform arbitrary transform?
		//auto pt = fTransform.mapPoint(fFrame.x, fFrame.y);
		//ctx->translate(pt.x, pt.y);

		drawBackground(fSurface);
		drawChildren(fSurface);
		drawSelf(fSurface);
		drawForeground(fSurface);

		//ctx->noClip();
		ctx->pop();

		compose(ctx);
	}

	void compose(std::shared_ptr<IDrawingContext> ctx)
	{
		ctx->image(fSurface->getImage(), (int)fFrame.x, (int)fFrame.y);
		//ctx->image(fSurface->getImage(), 0, 0);
	}

	void setTitle(const std::string& title)
	{
		fTitle = title;
	}

	bool inTitleBar(int x, int y)
	{
		return ((x >= fTitleBar.x) && (y >= fTitleBar.y) &&
			(x - fTitleBar.x <= fTitleBar.width) &&
			(y - fTitleBar.y <= fTitleBar.height));
	}
	
	void drawTitleBar(std::shared_ptr<IDrawingContext> ctx)
	{
		// If there's no title, don't draw anything
		if (fTitle.empty())
			return;

		// A little rectangle to back title
		ctx->noStroke();
		ctx->fill(fTitleBarColor);
		ctx->rect(fTitleBar.x, fTitleBar.y, fTitleBar.width, fTitleBar.height);

		// Draw title
		ctx->fill(0);
		ctx->textAlign(ALIGNMENT::CENTER, ALIGNMENT::BASELINE);
		ctx->textSize(12);
		ctx->text(fTitle.c_str(), fTitleBar.x+fTitleBar.width/2, 16);
		ctx->flush();
	}

	void drawForeground(std::shared_ptr<IDrawingContext> ctx)
	{
		drawTitleBar(ctx);
	}

	virtual void mouseEvent(const MouseEvent& e)
	{
		//printf("GWindow.mouseEvent: %d (%d,%d)\n", e.activity, e.x, e.y);

		switch (e.activity)
		{
		case MOUSEPRESSED:
			if (inTitleBar(e.x, e.y))
			{
				fIsMoving = true;
				fLastMouse = { (double)e.x, (double)e.y };

				return;
			}
			break;

		case MOUSERELEASED:
			if (fIsMoving) {
				fIsMoving = false;
			}
			break;

		case MOUSEMOVED:
			if (fIsMoving) {
				// move
				auto dx = (e.x - fLastMouse.x);
				auto dy = (e.y - fLastMouse.y);
				//printf("GWindow.mouseEvent(moved): %3.2f %3.2f\n", dx, dy);

				moveBy(dx, dy);

				fLastMouse = { (double)e.x-dx, (double)e.y-dy };
				return;
			}
			break;

		}

		// if we are here, the window itself did not 
		// handle the event, so allow sub-graphics to deal with it
		auto win = graphicAt(e.x, e.y);

		if (win) {
			win->mouseEvent(e);
		}
	}


};