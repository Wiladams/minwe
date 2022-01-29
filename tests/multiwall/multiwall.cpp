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

	This is also a perfect place to check if multi
	threading will make a significant performance 
	difference.

	A work stealing strategy might work out where each
	thread grabs a sub-section of the screen to pull
	pixels from.
*/
#include "gui.h"
#include "sampledraw2d.h"
#include "screensnapshot.h"
#include "normalizedwindow.h"

static const int captureWidth = 1280;
static const int captureHeight = 1080;

constexpr int maxCols = 32;
constexpr int maxRows = 32;

int numCols = 1;
int numRows = 1;

const int FRAMERATE = 30;

std::shared_ptr<ScreenSnapshot> snapper = std::make_shared<ScreenSnapshot>(40, 158, 640, 450);
SampledWindow winMan;

void resetCells()
{
	int cellWidth = canvasWidth / numCols;
	int cellHeight = canvasHeight / numRows;

	winMan.clearChildren();

	for (int row = 0; row < numRows; row++)
	{
		for (int col = 0; col < numCols; col++) {
			int x = col * cellWidth;
			int y = row * cellHeight;
			PixelRect frame(x, y, cellWidth, cellHeight);
			auto childFrame = TexelRect::create(frame, gAppSurface->frame());
			auto subframe = std::make_shared<SubSample>(snapper, TexelRect(0, 0, 1, 1));

			auto child = std::make_shared<SampledWindow>(subframe);
			child->setFrame(childFrame);

			winMan.addChild(child);
		}
	}
}

void keyReleased(const KeyboardEvent& e)
{
	switch (e.keyCode) {
	case VK_RIGHT:
		if (numCols < maxCols)
			numCols++;
		resetCells();
		break;
	case VK_LEFT:
		if (numCols > 1)
			numCols--;
		resetCells();
		break;

	case VK_DOWN:
		if (numRows > 1)
			numRows--;
		resetCells();
		break;
	case VK_UP:
		if (numRows < maxRows)
			numRows++;
		resetCells();
		break;
	}
}

void onFrame()
{
	snapper->next();

	sampleRectangle(*gAppSurface, gAppSurface->frame(), winMan);
}

void setup()
{
	setCanvasSize(captureWidth, captureHeight);
	//setCanvasSize(3840, 2160);
	setFrameRate(FRAMERATE);

	resetCells();
}