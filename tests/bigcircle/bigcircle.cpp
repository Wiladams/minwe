//
// bigcircle
// Demonstrate using the circle drawing routine, with sampling
//
// Commands:
// Use keyboard Arrow Keys 
//   UP,RIGHT - expand circle
//   DOWN,LEFT - contract circle
//
#include "gui.h"
#include "checkerboard.h"
#include "sampledraw2d.h"
#include "screensnapshot.h"

constexpr int minRadius = 40;
int Radius = minRadius;

// Drawing circle with 8-fold symmetry
// 
//http://www.mindcontrol.org/~hplus/graphics/RasterizeCircle.html
void hline(PixelMap& pmap, int cx, int y, int w, const PixelRGBA& c)
{
	for (int x = cx; x < cx + w; x++)
		pmap.set(x, y, c);
}

// fillin a circle given 8-fold symmetry
// using midpoint as start
void plot8(PixelMap& pmap, int cx, int cy, int x, int y, const PixelRGBA &c) 
{
	int dx = 2 * x;
	// bottom curve
	//pmap.set(cx+x, cy+y,c); pmap.set(cx-x, cy+y,c);
	hline(pmap, cx -x, cy + y, dx, c);

	// top curve
	//pmap.set(cx+x, cy-y, c); pmap.set(cx-x, cy-y, c);
	hline(pmap, cx - x, cy - y, dx, c);

	dx = 2 * y;
	// right and left cheeks
	//pmap.set(cx+y, cy+x, c); pmap.set(cx-y, cy+x, c);
	hline(pmap, cx - y, cy + x, dx, c);

	// right and left temples
	//pmap.set(cx+y, cy-x, c); pmap.set(cx-y, cy-x, c);
	hline(pmap, cx - y, cy - x, dx, c);
}

// draw a circle using 8-fold symmetry
void originCircle(PixelMap& pmap, int cx, int cy, int radius, const PixelRGBA& c)
{
	int rs2 = radius * radius * 4; /* this could be folded into ycs2 */
	int xs2 = 0;
	int ys2m1 = rs2 - 2 * radius + 1;
	int x = 0;
	int y = radius;
	int ycs2;

	plot8(pmap, cx, cy, x, y, c);
	while (x <= y) {
		/* advance to the right */
		xs2 = xs2 + 8 * x + 4;
		++x;
		/* calculate new Yc */
		ycs2 = rs2 - xs2;
		if (ycs2 < ys2m1) {
			ys2m1 = ys2m1 - 8 * y + 4;
			--y;
		}
		plot8(pmap,cx, cy, x, y,c);
	}
}



// We could create an instance of the checker pattern 
// globally, and not recreate it from scratch on each
// frame, but it's a fairly light weigth object
CheckerPattern circlePattern(PixelRGBA(0xff0000000), PixelRGBA(0xffffffff), 8);

std::shared_ptr<ScreenSnapshot> screenSamp = nullptr;

void keyReleased(const KeyboardEvent& e)
{
	switch (e.keyCode) {
	case VK_ESCAPE:
		halt();
		break;
	}

}

// Why use keyPressed instead of keyReleased?
// keyReleased() is called only once when you let go 
// of a key.  If you want continuous action, keyPressed()
// is called multiple times while you hold a key down.
//
void keyPressed(const KeyboardEvent& e)
{
	switch (e.keyCode) {

	case VK_UP:
	case VK_RIGHT:
		Radius += 10;
		break;

	case VK_DOWN:
	case VK_LEFT:
		Radius -= 10;
		if (Radius < minRadius)
			Radius = minRadius;
		break;
	}

}

// Each time, we'll draw the picture anew, 
// starting with a generic transparent background
// If you select the 'fullscreen' mode, then the circle
// will show on the screen without a window
void onFrame()
{
	background(PixelRGBA(0));

	screenSamp->next();

	//sampleCircle(*gAppSurface, canvasWidth / 2, canvasHeight / 2, Radius, circlePattern);
	//sampleCircle(*gAppSurface, canvasWidth / 2, canvasHeight / 2, Radius, *screenSamp);
	originCircle(*gAppSurface, canvasWidth / 2, canvasHeight / 2, Radius, PixelRGBA(255, 0, 0, 127));
}

void setup()
{
	setFrameRate(30);

	setCanvasSize(800, 800);
	//setCanvasSize(displayWidth, displayHeight);
	//fullscreen();

	// setup the snapshot
	screenSamp = std::make_shared<ScreenSnapshot>(0, 0, displayWidth / 2, displayHeight / 2);
}