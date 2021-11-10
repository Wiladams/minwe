#include "gui.h"

#include "targa.h"
#include "imagesampler.h"
#include "rendercontext.h"


void setup()
{
	User32PixelMap img;

	targa::initFromFile(img, "..\\debug\\shuttle.tga");
	//targa::initFromFile(img, "..\\debug\\money-256.tga");
	//targa::initFromFile(img, "..\\debug\\marbles.tga");
	

	//setCanvasSize(img.width(), img.height());
	setCanvasSize(img.width() * 2, img.height() * 2);

	RenderContext ctx(gAppSurface);

	auto isamp1 = std::make_shared<ImageSampler>(img);

	ctx.rectangle(0, 0, canvasWidth, canvasHeight, *isamp1);
}