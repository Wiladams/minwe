#include <memory>

#include "gui.h"
#include "rendercontext.h"
#include "screensnapshot.h"
#include "recorder.h"
#include "stopwatch.h"


std::shared_ptr<RenderContext> ctx = nullptr;
std::shared_ptr<ScreenSnapshot> screenSamp = nullptr;
std::shared_ptr<Recorder> reco = nullptr;

void keyReleased(const KeyboardEvent &e)
{
	if (e.keyCode == VK_ESCAPE)
		halt();

	if (e.keyCode == VK_SPACE)
		reco->toggleRecording();
}

void drawPoints(std::shared_ptr<RenderContext> ctx)
{
	// Draw some random sampled points
	for (int i = 0; i < 25000; i++)
	{
		int x = maths::random(canvasWidth-1);
		int y = maths::random(canvasHeight-1);
		double u = maths::Map(x, 0, canvasWidth-1, 0, 1);
		double v = maths::Map(y, 0, canvasHeight-1, 0, 1);

		auto c = screenSamp->getValue(u, v, { x,y });
		//ctx->point(x, y, *screenSamp);
		//ctx->circle(x, y, 2, *screenSamp);
		fillCircle(*gAppSurface, x, y, 6, c);
		//gAppSurface->setPixel(x, y, c);

	}
}

void onFrame()
{
	screenSamp->next();

	// start with blank slate
	background(0);

	drawPoints(ctx);

	// Draw a rectangle or two
	//ctx->rectangle(100, 100, 400, 300, *screenSamp);
	//ctx->rectangle(300, 400, 640, 480, *screenSamp);

	reco->saveFrame();
}

void setup()
{
	setFrameRate(30);
	setCanvasSize(800, 600);

	screenSamp = std::make_shared<ScreenSnapshot>(100, 400, canvasWidth, canvasHeight);
	ctx = std::make_shared<RenderContext>(gAppSurface);

	// setup the recorder
	reco = std::make_shared<Recorder>(&(*gAppSurface), "frame-", 0);
}