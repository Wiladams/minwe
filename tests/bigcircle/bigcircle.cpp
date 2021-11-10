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

// We could create an instance of the checker pattern 
// globally, and not recreate it from scratch on each
// frame, but it's a fairly light weigth object
CheckerPattern circlePattern(0xff0000000, 0xffffffff, 8);

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
	background(0);

	screenSamp->next();

	//sampleCircle(*gAppSurface, canvasWidth / 2, canvasHeight / 2, Radius, circlePattern);
	sampleCircle(*gAppSurface, canvasWidth / 2, canvasHeight / 2, Radius, *screenSamp);

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