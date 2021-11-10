#include "gui.h"
#include "draw2d.h"
#include "checkerboard.h"
#include "sampledraw2d.h"

int xRadius = 40;
int yRadius = 40;


void keyReleased(const KeyboardEvent& e)
{
	switch (e.keyCode) {
	case VK_ESCAPE:
		halt();
		break;
	}

}

void keyPressed(const KeyboardEvent& e)
{
	switch (e.keyCode) {

	case VK_UP:
		yRadius += 10;
		break;

	case VK_DOWN:
		yRadius -= 10;
		if (yRadius < 10)
			yRadius = 10;
		break;

	case VK_RIGHT:
		xRadius += 10;
		break;

	case VK_LEFT:
		xRadius -= 10;
		if (xRadius < 10)
			xRadius = 10;
		break;
	}

	//printf("xRadius: %d yRadius: %d\n", xRadius, yRadius);

}

void onFrame()
{
	int cx = canvasWidth / 2;
	int cy = canvasHeight / 2;

	background(0);

	//fillRectangle(*gAppSurface, cx - xRadius, cy - yRadius, xRadius * 2, yRadius * 2, 0xff0000ff);

	fillEllipse(*gAppSurface, cx, cy, xRadius, yRadius, 0xffff0000);
	//strokeEllipse(*gAppSurface, cx, cy, xRadius, yRadius, 0xffffffff);
	
	//CheckerPattern chkbg(0xff0c0c0c,0xff1c1c1c, 8);
	//sampleCircle(*gAppSurface, cx, cy, xRadius, chkbg);

}

void setup()
{
	//setCanvasSize(displayWidth, displayHeight);
	fullscreen();
}