#pragma once

#include "pixelmap.h"

// Enumerations for all the commands
// 
enum  GRCOMMANDS {
	GCMD_NONE,
	GCMD_ANGLEMODE,
	GCMD_ELLIPSEMODE,
	GCMD_RECTMODE,
	GCMD_BLENDMODE,
	GCMD_STROKECAPS,
	GCMD_STROKEJOIN,
	GCMD_STROKEWEIGHT,

	GCMD_PUSH,
	GCMD_POP,

	GCMD_TRANSLATE,
	GCMD_SCALE,
	GCMD_ROTATE,

	GCMD_FILL_COLOR,
	GCMD_FILL_GRADIENT,
	GCMD_FILL_PATTERN,
	GCMD_FILL_NONE,

	GCMD_STROKE,
	GCMD_NOSTROKE,

	GCMD_FLUSH,
	GCMD_LOADPIXELS,
	GCMD_UPDATEPIXELS,
	GCMD_CLEAR,
	GCMD_CLEARRECT,
	GCMD_BACKGROUND,
	GCMD_CLIP,
	GCMD_NOCLIP,

	GCMD_SET,
	GCMD_POINT,
	GCMD_LINE,
	GCMD_RECT,
	GCMD_ROUNDRECT,
	GCMD_ELLIPSE,
	GCMD_CIRCLE,
	GCMD_TRIANGLE,
	GCMD_BEZIER,
	GCMD_POLYLINE,
	GCMD_POLYGON,
	GCMD_QUAD,
	GCMD_PATH,

	GCMD_IMAGE,
	GCMD_IMAGE_SCALE,

	GCMD_TEXTALIGN,
	GCMD_TEXTFONT,
	GCMD_TEXTSIZE,
	GCMD_TEXT,

	GCMD_BEGINSHAPE,
	GCMD_VERTEX,
	GCMD_ENDSHAPE,
};

// RectMode
enum class RECTMODE : unsigned
{
	CORNER,
	CORNERS,
	CENTER,
};

// EllipseMode
enum class ELLIPSEMODE : unsigned
{
	CORNER,
	CORNERS,
	CENTER,
	RADIUS
};

// Text Alignment
enum class ALIGNMENT : unsigned
{
	CENTER = 0x01,

	LEFT = 0x02,
	RIGHT = 0x04,

	BASELINE = 0x20,
	TOP = 0x10,
	BOTTOM = 0x40,

};

// are angles specified in degrees
// or in radians
enum ANGLEMODE : unsigned
{
	DEGREES,
	RADIANS
};

enum class SHAPEMODE : unsigned
{
	NONE = 0,
	OPEN = 1,
	POINTS = 2,
	LINES = 3,
	TRIANGLES = 4,
	TRIANGLE_STRIP = 5,
	TRIANGLE_FAN = 6,
	QUADS = 7,
	QUAD_STRIP = 8,

};


enum class SHAPEEND : unsigned
{
	STROKE = 0,
	CLOSE = 1,
};

enum class DRAWSTYLE : unsigned
{
	Fill,
	Stroke,
	FillAndStroke,
	StrokeAndFill
};


typedef PixelRGBA Pixel;


/*
	IGraphics defines the essential interface for doing vector graphics
	This is a pure virtual interface for the most part, so a sub-class must
	implement a fair bit of stuff.
*/
class IDrawingContext
{
public:
	//virtual ~IGraphics() {}

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
