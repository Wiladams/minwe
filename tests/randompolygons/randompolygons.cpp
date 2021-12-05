#include "apphost.h"
#include "draw2d.h"

void onLoad()
{
	setCanvasSize(800, 600);

	PixelCoord verts[] = { {10,10},{300,10},{300,300},{10,300} };
	fillConvexPolygon(*gAppSurface, verts, 4, 0, 0xffffff00, {0,0,canvasWidth,canvasHeight});

	refreshScreen();
}