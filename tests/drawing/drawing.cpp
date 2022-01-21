#include "apphost.h"

#include "draw2d.h"

#define black	PixelRGBA(0xff000000)
#define white	PixelRGBA(0xffffffff)
#define red		PixelRGBA(0xffff0000)
#define green	PixelRGBA(0xff00ff00)
#define blue	PixelRGBA(0xff0000ff)
#define yellow	PixelRGBA(0xffffff00)

void onLoad()
{
	// clear screen to white
	gAppSurface->setAllPixels(white);

	fillRectangle(*gAppSurface, 5, 5, 205, 205, yellow);
	strokeRectangle(*gAppSurface, 5, 5, 205, 205, black);

	line(*gAppSurface, 10, 10, 210, 10, red);

	line(*gAppSurface, 10, 10, 210, 200, blue);

	line(*gAppSurface, 10, 200, 210, 200, green);
}