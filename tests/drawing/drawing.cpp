#include "apphost.h"

#include "draw2d.h"

#define black	0xff000000
#define white	0xffffffff
#define red		0xffff0000
#define green	0xff00ff00
#define blue	0xff0000ff
#define yellow	0xffffff00

void onLoad()
{
	// clear screen to white
	gAppSurface->setAllPixels(white);

	fillRectangle(*gAppSurface, 5, 5, 205, 205, PixelRGBA(yellow));
	strokeRectangle(*gAppSurface, 5, 5, 205, 205, PixelRGBA(black));

	line(*gAppSurface, 10, 10, 210, 10, PixelRGBA(red));

	line(*gAppSurface, 10, 10, 210, 200, PixelRGBA(blue));

	line(*gAppSurface, 10, 200, 210, 200, PixelRGBA(green));
}