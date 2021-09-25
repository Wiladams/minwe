#pragma once


#include "drawingcontext.h"
#include "User32PixelMap.h"

class PixelSurface : public IDrawingContext
{

	User32PixelMap fPixelMap;

	// State variables
	int fAngleMode = RADIANS;
	ELLIPSEMODE fEllipseMode = ELLIPSEMODE::RADIUS;
	RECTMODE fRectMode = RECTMODE::CORNER;

	bool fUseFill = true;
	// Drawing Attributes
	double fDimensionScale = 1.0;

	// Text Stuff
	//BLFontFace fFontFace;
	//BLFont fFont;
	float fFontSize = 12;
	ALIGNMENT fTextHAlignment = ALIGNMENT::LEFT;
	ALIGNMENT fTextVAlignment = ALIGNMENT::BASELINE;

	// Vertex shaping
	SHAPEMODE fShapeMode = SHAPEMODE::NONE;
	std::vector<PixelCoord> fShapeVertices;

	Pixel fFillColor = PixelRGBA(0, 0, 0, 255);
	Pixel fStrokeColor = PixelRGBA(0, 0, 0, 255);
	Pixel fTextFillColor = PixelRGBA(0, 0, 0, 255);


private:
	static PixelEllipse calcEllipseParams(ELLIPSEMODE mode, double& a, double& b, double& c, double& d)
	{
		double cx = 0;
		double cy = 0;
		double rx = 0;
		double ry = 0;

		switch (mode) {
		case ELLIPSEMODE::CORNER:
			rx = c / 2;
			ry = d / 2;
			cx = a + rx;
			cy = b + ry;
			break;
		case ELLIPSEMODE::CORNERS:
			rx = (c - a) / 2;
			ry = (d - b) / 2;
			cx = a + rx;
			cy = b + ry;
			break;
		case ELLIPSEMODE::CENTER:
			rx = c / 2;
			ry = d / 2;
			cx = a;
			cy = b;
			break;
		case ELLIPSEMODE::RADIUS:
			cx = a;
			cy = b;
			rx = c;
			ry = d;
		}

		return { cx, cy, rx, ry };
	}

	static PixelRect calcRectParams(RECTMODE mode, double& a, double& b, double& c, double& d)
	{
		double x = 0;
		double y = 0;
		double w = 0;
		double h = 0;

		switch (mode) {
		case RECTMODE::CORNER:
			x = a;
			y = b;
			w = c;
			h = d;
			break;
		case RECTMODE::CORNERS:
			x = a;
			y = b;
			w = c - a;
			h = d - b;
			break;
		case RECTMODE::CENTER:
			x = a - (c / 2);
			y = b - (d / 2);
			w = c;
			h = d;
			break;
		}

		return { x, y, w, h };
	}

public:
	PixelSurface(int width, int height)
		:fPixelMap(width, height)
	{

	}

	inline int getWidth() { return fPixelMap.width; }
	inline int getHeight() { return fPixelMap.height; }
	inline PixelRGBA* getData() { return fPixelMap.getData(); }

	inline PixelMap& getImage() { return fPixelMap; }

	inline void set(int x, int y, PixelRGBA c) { fPixelMap.set(x, y, c); }
	inline PixelRGBA get(int x, int y) { return fPixelMap.get(x, y); }

	// DrawingContext stuff
	virtual void angleMode(int newMode) = 0;
	virtual void ellipseMode(const ELLIPSEMODE mode) = 0;
	virtual void rectMode(const RECTMODE mode) = 0;
	virtual void blendMode(int mode) = 0;
	virtual void strokeCaps(int caps) = 0;
	virtual void strokeJoin(int join) = 0;
	virtual void strokeWeight(double weight) = 0;

	virtual void push() = 0;
	virtual void pop() = 0;
	virtual void flush() = 0;

	virtual void translate(double dx, double dy) = 0;
	virtual void scale(double sx, double sy) = 0;
	virtual void scale(double sxy) { scale(sxy, sxy); }
	virtual void rotate(double angle, double cx, double cy) = 0;
	virtual void rotate(double angle) { rotate(angle, 0, 0); }

	virtual int blue(const Pixel& c) { return c.blue; }
	virtual int green(const Pixel& c) { return c.green; }
	virtual int red(const Pixel& c) { return c.red; }
	virtual int alpha(const Pixel& c) { return c.alpha; }

	// BUGBUG, only handles RGB mode, not HSL
	virtual Pixel color(int a, int b, int c, int d) { return Pixel(a, b, c, d); }
	virtual Pixel color(int r, int g, int b) { return color(r, g, b, 255); }
	virtual Pixel color(int gray, int alpha) { return color(gray, gray, gray, alpha); }
	virtual Pixel color(int gray) { return color(gray, gray, gray, 255); }

	virtual Pixel lerpColor(const Pixel& from, const Pixel& to, double f) {
		//auto p = maths::Lerp(f, from, to);
		//return p;

		uint8_t r = (uint8_t)maths::Lerp(f, (double)from.red, (double)to.red);
		uint8_t g = (uint8_t)maths::Lerp(f, (double)from.green, (double)to.green);
		uint8_t b = (uint8_t)maths::Lerp(f, (double)from.blue, (double)to.blue);
		uint8_t a = (uint8_t)maths::Lerp(f, (double)from.alpha, (double)to.alpha);

		return Pixel((int)r, (int)g, (int)b, (int)a);
	}

	//virtual void fill(const BLGradient& g) = 0;
	virtual void fill(const Pixel& c) = 0;
	virtual void fill(int r, int g, int b, int a) { fill(color(r, g, b, a)); }
	virtual void fill(int r, int g, int b) { fill(color(r, g, b, 255)); }
	virtual void fill(int gray, int alpha) { fill(color(gray, gray, gray, alpha)); }
	inline virtual void fill(int gray) { fill(color(gray, gray, gray, 255)); }
	virtual void noFill() = 0;

	virtual void stroke(const Pixel& c) = 0;
	virtual void stroke(int r, int g, int b, int a) { stroke(color(r, g, b, a)); }
	virtual void stroke(int r, int g, int b) { stroke(color(r, g, b, 255)); }
	virtual void stroke(int gray, int alpha) { stroke(color(gray, gray, gray, alpha)); }
	virtual void stroke(int gray) { stroke(color(gray, gray, gray, 255)); }
	virtual void noStroke() = 0;

	// Background management
	virtual void clear() = 0;
	virtual void clearRect(double x, double y, double w, double h) = 0;
	virtual void background(const Pixel& c) = 0;

	// Clipping
	virtual void clip(double x, double y, double w, double h) = 0;
	virtual void noClip() = 0;

	// Geometry
	// hard set a specfic pixel value
	virtual void set(double x, double y, const Pixel& c) = 0;
	virtual void point(double x, double y) = 0;
	virtual void line(double x1, double y1, double x2, double y2) = 0;
	virtual void rect(double x, double y, double width, double height) { rect(x, y, width, height, 1, 1); }
	virtual void rect(double x, double y, double width, double height, double xradius, double yradius) = 0;
	virtual void ellipse(double a, double b, double c, double d) = 0;
	virtual void circle(double cx, double cy, double diameter) = 0;
	virtual void triangle(double x1, double y1, double x2, double y2, double x3, double y3) = 0;
	virtual void bezier(double x1, double y1, double x2, double y2, double x3, double y3, double x4, double y4) = 0;
	virtual void polyline(const PixelCoord* pts, size_t n) = 0;
	virtual void polygon(const PixelCoord* pts, size_t n) = 0;
	virtual void quad(double x1, double y1, double x2, double y2, double x3, double y3, double x4, double y4) = 0;

	//virtual void path(const BLPath& path) = 0;
	//virtual void strokePath(const BLPath& path) = 0;
	//virtual void fillPath(const BLPath& path) = 0;

	virtual void beginShape(SHAPEMODE shapeKind) = 0;
	virtual void vertex(double x, double y) = 0;
	virtual void endShape(SHAPEEND endKind) = 0;

	// Bitmaps
	virtual void image(const PixelMap& img, int x, int y) = 0;
	virtual void scaleImage(const PixelMap& src,
		double srcX, double srcY, double srcWidth, double srcHeight,
		double dstX, double dstY, double dstWidth, double dstHeight) = 0;
	/*
	void scaleImage(BLImage& src, double x, double y, double scaleX, double scaleY)
	{
		double dstWidth = src.width() * scaleX;
		double dstHeight = src.height() * scaleY;

		BLRect dst{ x,y,dstWidth,dstHeight };
		BLRectI srcArea{ 0,0,src.width(),src.height() };

		gAppDC.blitImage(dst, src, srcArea);
	}
	*/

	virtual void textAlign(ALIGNMENT horizontal, ALIGNMENT vertical) = 0;
	virtual void textFont(const char* fontname) = 0;
	virtual void textSize(double size) = 0;
	virtual void text(const char* txt, double x, double y) = 0;
};