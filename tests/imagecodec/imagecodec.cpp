#include "gui.h"
#include "recorder.h"

Recorder* reco = nullptr;

void onFrame()
{
	reco->saveFrame();
}


void setup()
{
	setCanvasSize(640, 480);
	setFrameRate(5);

	fillRectangle(*gAppSurface, 10, 10, 400, 300, PixelRGBA(0xffff0000));

	reco = new Recorder(&(*gAppSurface), "frame", 0);
	reco->record();
}