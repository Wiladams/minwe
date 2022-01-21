#include "apphost.h"
#include "draw2d.h"

void onLoad()
{
	setCanvasSize(800, 600);

	PixelCoord verts[] = { PixelCoord({10,10}),PixelCoord({300,10}),PixelCoord({300,300}),PixelCoord({10,300}) };
	fillConvexPolygon(*gAppSurface, verts, 4, 0, PixelRGBA(0xffffff00), PixelRect(0,0,canvasWidth,canvasHeight));

	refreshScreen();
}