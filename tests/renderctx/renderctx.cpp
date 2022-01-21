#include <memory>

#include "gui.h"
#include "rendercontext.h"
#include "screensnapshot.h"
#include "recorder.h"
#include "stopwatch.h"

using namespace maths;

std::shared_ptr<RenderContext> ctx = nullptr;
std::shared_ptr<ScreenSnapshot> screenSamp = nullptr;
std::shared_ptr<Recorder> reco = nullptr;

int maxElements = 1000;

void keyReleased(const KeyboardEvent &e)
{
	if (e.keyCode == VK_ESCAPE)
		halt();

	if (e.keyCode == VK_SPACE)
		reco->toggleRecording();
}

void keyPressed(const KeyboardEvent& e)
{
	switch (e.keyCode) {
	case VK_UP:
		maxElements += 1000;
		break;
	case VK_DOWN:
		maxElements -= 1000;
		if (maxElements < 1000)
			maxElements = 1000;
		break;
	}
	printf("maxElements: %d\n", maxElements);
}
void drawPoints(std::shared_ptr<RenderContext> ctx)
{
	int stemSize = Clamp(Map(maxElements, 1000, 200000, 30, 2), 2,20);
	int stemHalf = stemSize / 2;

	// Draw some random sampled points
	for (int i = 0; i < maxElements; i++)
	{
		int x = random(canvasWidth-1);
		int y = random(canvasHeight-1);
		double u = Map(x, 0, canvasWidth-1, 0, 1);
		double v = Map(y, 0, canvasHeight-1, 0, 1);

		// get a random sample from the screen capture
		auto c = screenSamp->getValue(u, v, PixelCoord({ x,y }));

		// turn that into a couple of styled lines
		//fillCircle(*gAppSurface, x, y, stemHalf, c);
		
		//gAppSurface->set(x, y, c);

		//fillTriangle(*gAppSurface, x, y - stemHalf, x + stemHalf, y + stemHalf, x - stemHalf, y + stemHalf, c, { 0,0,canvasWidth,canvasHeight });

		//fillRectangle(*gAppSurface, x - stemHalf, y, stemSize, 1, c);
		//fillRectangle(*gAppSurface, x, y - stemHalf, 1, stemSize, c);

		fillRectangle(*gAppSurface, x - stemHalf, y-stemHalf, stemSize, stemSize, c);

	}
}

// some other effects that can be done above

//ctx->point(x, y, *screenSamp);
//ctx->circle(x, y, 2, *screenSamp);


void onFrame()
{
	screenSamp->next();

	// start with blank slate
	background(PixelRGBA(0));

	drawPoints(ctx);

	reco->saveFrame();
}

void setup()
{
	setFrameRate(15);
	setCanvasSize(800, 600);

	screenSamp = std::make_shared<ScreenSnapshot>(100, 400, canvasWidth, canvasHeight);
	ctx = std::make_shared<RenderContext>(gAppSurface);

	// setup the recorder
	reco = std::make_shared<Recorder>(&(*gAppSurface), "frame-", 0);
}