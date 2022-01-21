/*
	Demonstration of instance sampling
	Take a snapshot of the screen as the source
	and replicate it multiple times across the screen

	Control
	Up Arrow - Add more rows, up to 16
	Right Arrow - Add more columns, up to 16

	Down, Left - Reduce rows, columns

	Using a Youtube window pegged to the left edge
	of the monitor works well.
*/
#include "gui.h"
#include "sampledraw2d.h"
#include "screensnapshot.h"

static const int captureWidth = 1280;
static const int captureHeight = 1080;

constexpr int maxCols = 16;
constexpr int maxRows = 16;

int numCols = 1;
int numRows = 1;

ScreenSnapshot snapper(40, 158, 640, 450);

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
			/*
			PixelCoord verts[] = { 
				PixelCoord({x,y}),
				PixelCoord({x+cellWidth,y}),
				PixelCoord({x+cellWidth,y+cellHeight}),
				PixelCoord({x,y+cellHeight}) 
			};
			int nverts = 4;
			sampleConvexPolygon(*gAppSurface,
				verts, nverts, 0,
				snapper,
				{ 0,0,canvasWidth, canvasHeight });
			*/
			sampleRect(*gAppSurface,
				PixelRect(x, y,cellWidth, cellHeight), TexelRect(0,0,1,1),
				snapper);
		}
	}
}

void setup()
{
	setCanvasSize(captureWidth*2, captureHeight);
	//setCanvasSize(3840, 2160);

}