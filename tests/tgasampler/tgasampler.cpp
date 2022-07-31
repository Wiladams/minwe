#include "gui.h"
#include "targa.h"
#include "rendercontext.h"

void setup()
{
	User32PixelMap img;

	targa::initFromFile(img, "..\\debug\\shuttle.tga");
	//targa::initFromFile(img, "..\\debug\\money-256.tga");
	//targa::initFromFile(img, "..\\debug\\marbles.tga");
	
	setCanvasSize(img.width(), img.height());
	setTitle("Targa Sampler");

	RenderContext ctx(gAppSurface);

	ctx.rectangle(PixelRect(0, 0, canvasWidth, canvasHeight), img);
}