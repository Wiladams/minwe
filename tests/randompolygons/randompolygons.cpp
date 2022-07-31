#include "apphost.h"
#include "draw.h"

PixelArray gpa;
PixelPolygon gpoly;

void onLoad()
{
	setCanvasSize(800, 600);
	
	gpa.init(canvasPixels, canvasWidth, canvasHeight, canvasBytesPerRow);
	
	PixelCoord verts[] = { PixelCoord({10,10}),PixelCoord({300,10}),PixelCoord({300,300}),PixelCoord({10,300}) };
	gpoly.addPoints(verts,4);
	
	draw::copyPolygon(gpa, gpoly, PixelRGBA(0xffffff00));
}