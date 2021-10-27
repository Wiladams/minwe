#include "gui.h"
#include "user32pixelmap.h"
#include "targa.h"
#include "imagesampler.h"

void transferImageSamples(std::shared_ptr<ImageSampler> samp)
{
	for (int row=0;row<canvasHeight;row++)
		for (int col = 0; col < canvasWidth; col++)
		{
			double u = maths::Map(col, 0, canvasWidth - 1, 0, 1);
			double v = maths::Map(row, 0, canvasHeight - 1, 0, 1);

			auto c = samp->getValue(u, v);
			gAppSurface->set(col, row, c);
		}
}

void setup()
{
	User32PixelMap img;
	//targa::initFromFile(img, "..\\debug\\shuttle.tga");
	targa::initFromFile(img, "..\\debug\\money-256.tga");
	//targa::initFromFile(img, "..\\debug\\marbles.tga");
	auto isamp1 = std::make_shared<ImageSampler>(img);

	setCanvasSize(img.getWidth()*2, img.getHeight()*2);

	transferImageSamples(isamp1);
}