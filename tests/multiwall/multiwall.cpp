#include "gui.h"
#include "sampledraw2d.h"
#include "screensnapshot.h"

static const int captureWidth = 1280;
static const int captureHeight = 1080;

constexpr int maxCols = 8;
constexpr int maxRows = 8;

int numCols = 1;
int numRows = 1;

ScreenSnapshot snapper(0, 0, captureWidth, captureHeight);

void keyReleased(const KeyboardEvent& e)
{
	switch (e.keyCode) {
	case VK_RIGHT:
		if (numCols < maxCols)
			numCols++;
		break;
	case VK_LEFT:
		if (numCols > 1)
			numCols--;
		break;

	case VK_DOWN:
		if (numRows > 1)
			numRows--;
		break;
	case VK_UP:
		if (numRows < maxRows)
			numRows++;
		break;
	}
}

void onFrame()
{
	int cellWidth = canvasWidth / numCols;
	int cellHeight = canvasHeight / numRows;

	snapper.next();

	for (int row = 0; row < numRows; row++)
	{
		for (int col = 0; col < numCols; col++) {
			int x = col * cellWidth;
			int y = row * cellHeight;
			PixelCoord verts[] = { 
				{x,y},
				{x+cellWidth,y},
				{x+cellWidth,y+cellHeight},
				{x,y+cellHeight} 
			};
			int nverts = 4;
			//sampleConvexPolygon(*gAppSurface,
			//	verts, nverts, 0,
			//	snapper,
			//	{ 0,0,canvasWidth, canvasHeight });
			sampleRectangle(*gAppSurface,
				x, y,
				cellWidth, cellHeight,
				snapper);
		}
	}
}

void setup()
{
	setCanvasSize(captureWidth*2, captureHeight);
	//setCanvasSize(3840, 2160);

}