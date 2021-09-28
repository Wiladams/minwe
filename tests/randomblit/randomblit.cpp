#include "apphost.h"
#include "draw2d.h"
#include "gui.h"
#include "filestream.h"
#include "targa.h"

User32PixelMap src(320, 240);

void setup()
{


	setCanvasSize(1024, 768);

	// set the canvas to white
	gAppSurface->setAllPixels(0xffffffff);

	// create the source as a solid color
	src.setAllPixels(0xffffff00);





	// Now do the blit
	blit(*gAppSurface, 10, 10, src);

	blit(*gAppSurface, 824, 10, src);

	// partial bottom
	blit(*gAppSurface, 824, 668, src);

	// out of frame
	blit(*gAppSurface, 300, 770, src);

	// partial left
	blit(*gAppSurface, -200, 300, src);

	targa::TargaMeta meta;
	PixelMap* img = targa::readFromFile("d:\\repos\\minwe\\tests\\debug\\shuttle.tga", meta);
	if (img != nullptr) {
		blit(*gAppSurface, 200, 200, *img);
	}

	refreshScreen();
}