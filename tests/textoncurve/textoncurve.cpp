#include "gui.h"
#include "geotypes.hpp"
#include "textlayout.h"

using namespace alib;

constexpr int margin = 50;
constexpr int FRAMERATE = 20;

int dir = 1;				// direction
int currentIteration = 1;	// Changes during running
int iterations = 30;		// increase past frame rate to slow down
bool showCurve = true;
TextLayout tLayout;

void textOnBezier(const char* txt, GeoBezier<ptrdiff_t>& bez)
{
	int len = strlen(txt);

	double u = 0.0;
	int offset = 0;
	int xoffset = 0;

	while (txt[offset])
	{
		// Isolate the current character
		char buf[2];
		buf[0] = txt[offset];
		buf[1] = 0;

		// Figure out the x and y offset
		auto pt = bez.eval(u);

		// Display current character
		tLayout.text(buf, pt.x(), pt.y());

		// Calculate size of current character
		// so we can figure out where next one goes
		PixelCoord charSize;
		tLayout.textMeasure(charSize, buf);

		// Now get the next value of 'u' so we 
		// can evaluate where the next character will go
		xoffset += charSize.x();
		u = bez.findUForX(xoffset);

		offset++;
	}

}

void textFillBezier(const char* txt, GeoBezier<ptrdiff_t>& bez)
{
	int len = strlen(txt);

	int offset = 0;

	while (txt[offset])
	{
		double tu = (double)offset / len;
		auto pt = bez.eval(tu);

		char buf[2];
		buf[0] = txt[offset];
		buf[1] = 0;

		// Go character by character
		// Display current character
		tLayout.text(buf, pt.x(), pt.y());

		offset++;
	}

}

void strokeCurve(PixelMap& pmap, GeoBezier<ptrdiff_t> &bez, int segments, const PixelRGBA c)
{
	// Get starting point
	auto lp = bez.eval(0.0);

	int i = 1;
	while (i <= segments) {
		double u = (double)i / segments;

		auto p = bez.eval(u);

		// draw line segment from last point to current point
		line(pmap, lp.x(), lp.y(), p.x(), p.y(), c);

		// Assign current to last
		lp = p;

		i = i + 1;
	}
}

void onFrame()
{
	background(PixelRGBA(0xffffffff));

	int y1 = maths::Map(currentIteration, 1, iterations, 0, canvasHeight);
	//GeoQuadraticBezier<ptrdiff_t> bez(margin, canvasHeight / 2, canvasWidth / 2, y1, canvasWidth - margin, canvasHeight / 2);
	GeoCubicBezier<ptrdiff_t> bez(margin, canvasHeight / 2, canvasWidth * 0.25, y1, canvasWidth - (canvasWidth * 0.25), canvasHeight -y1, canvasWidth - margin, canvasHeight / 2.0);
	bez.calcSpeeds();

	if (showCurve)
		strokeCurve(*gAppSurface, bez, 60, PixelRGBA(0xffff0000));

	// fit text to curve offsets
	//tLayout.textColor(PixelRGBA(0xff00ff00));
	//textFillBezier("When Will The Quick Brown Fox Jump Over the Lazy Dogs Back", bez);

	// honor the character spacing
	tLayout.textColor(PixelRGBA(0xff0000ff));
	textOnBezier("When Will The Quick Brown Fox Jump Over the Lazy Dogs Back", bez);


	currentIteration += dir;

	// reverse direction if needs be
	if ((currentIteration >= iterations) || (currentIteration <= 1))
		dir = dir < 1 ? 1 : -1;
}

void setup()
{
	setCanvasSize(800, 600);
	setFrameRate(FRAMERATE);

	tLayout.init(gAppSurface);
	tLayout.textFont("Jokerman", 24);
	tLayout.textAlign(ALIGNMENT::CENTER, ALIGNMENT::CENTER);
}


void keyReleased(const KeyboardEvent& e) 
{
	switch (e.keyCode) {
	case VK_ESCAPE:
		halt();
		break;

	case VK_SPACE:
		showCurve = !showCurve;
		break;

	case VK_UP:
		iterations += 1;
		break;

	case VK_DOWN:
		iterations -= 1;
		if (iterations < 2)
			iterations = 2;
		break;

	case 'R':
		recordingToggle();
		break;
	}
}
