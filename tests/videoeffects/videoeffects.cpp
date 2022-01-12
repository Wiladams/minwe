#include "gui.h"
#include "screensnapshot.h"
#include "recorder.h"
#include "stopwatch.h"
#include "rendercontext.h"

#include "effect_barndoor.h"
#include "effect_circlegrid.h"
#include "effect_slabs.h"
#include "effect_checkers.h"
#include "effect_crossfade.h"
#include "parametricrect.h"

using namespace maths;



// Source Samplers
std::shared_ptr<ScreenSnapshot> screen1 = nullptr;
std::shared_ptr<ScreenSnapshot> screen2 = nullptr;
std::shared_ptr<SolidColorSampler> transSampler = nullptr;

// Wrapping Samplers
std::shared_ptr<CheckerSampler> checkSamp = nullptr;
std::shared_ptr<GraySampler> graySamp = nullptr;
std::shared_ptr<EffectCheckers> checkersEffect = nullptr;

// Effects Samplers
std::shared_ptr<VisualEffect> blankEffect = nullptr;
std::shared_ptr<CrossFadeEffect> fadeFromBlack = nullptr;
std::shared_ptr<CrossFadeEffect> fadeToBlack = nullptr;
std::shared_ptr<CrossFadeEffect> fadeScreen1ToScreen2 = nullptr;
std::shared_ptr<BarnDoorOpenEffect> barnDoorOpen = nullptr;
std::shared_ptr<BarnDoorCloseEffect> barnDoorClose = nullptr;

std::shared_ptr<VisualEffect> currentEffect = nullptr;

std::shared_ptr<Recorder> reco = nullptr;
std::shared_ptr<RenderContext> ctx = nullptr;


constexpr int CAPTUREWIDTH = 800;
constexpr int CAPTUREHEIGHT = 800;
constexpr int FRAMERATE = 20;

int maxElements = 5000;
int frequency = 8;
int currentIteration = 0;	// Changes during running
int maxiterations = FRAMERATE;		// increase past frame rate to slow down
double progress = 0;

std::shared_ptr< ParametricShard> paramShard = nullptr;


void keyReleased(const KeyboardEvent& e)
{
	switch (e.keyCode)
	{
	case VK_ESCAPE:
		halt();
		break;

	case VK_SPACE:
		reco->toggleRecording();
		break;

	case VK_F1:
	case '1':
		currentEffect = fadeFromBlack;
		currentEffect->start();
		break;

	case VK_F2:
	case '2':
		// flip between screens
		currentEffect = fadeScreen1ToScreen2;
		currentEffect->start();
		break;

	case VK_F4:
	case '4':
		currentEffect = barnDoorOpen;
		currentEffect->start();
		break;

	case VK_F5:
	case '5':
		//barnDoorClose->setSource1(currentEffect->source2());
		//barnDoorClose->setSource2(currentEffect->source1());
		currentEffect = barnDoorClose;
		currentEffect->start();
		break;

	case VK_F10:
	case '0':
		fadeToBlack->setSource1(currentEffect->source2());
		currentEffect = fadeToBlack;
		currentEffect->start();
		break;
	}
}

void keyPressed(const KeyboardEvent& e)
{
	switch (e.keyCode) {
	case VK_UP:
		progress += 0.10;
		progress = maths::Clamp(progress, 0, 1);

		break;
	case VK_DOWN:
		progress -= 0.10;
		progress = maths::Clamp(progress, 0, 1);
		break;


	}
}


void onFrame()
{
	// If there's no effect, don't do anything
	// keep whatever was last on the screen;
	if (nullptr == currentEffect)
		return;

	screen1->next();
	screen2->next();

	// start with blank slate
	background(PixelRGBA(0));

	//checkersEffect->draw(gAppSurface);

	// Cross Fade
	currentEffect->update();
	//currentEffect->setProgress(progress);
	ctx->rect(gAppSurface->getBounds(), { 0,0,1,1 }, *currentEffect);



	//ctx->rect(gAppSurface->getBounds(), { 0,0,1,1 }, *screen1);
	//ctx->rect(gAppSurface->getBounds(), { 0,0,1,1 }, *screen2);


	reco->saveFrame();
}

void setup()
{
	setCanvasSize(displayWidth/2, displayHeight/2);
	setFrameRate(FRAMERATE);

	ctx = std::make_shared<RenderContext>(gAppSurface);

	transSampler = std::make_shared<SolidColorSampler>(0x00000000);

	screen1 = std::make_shared<ScreenSnapshot>(0, 0, displayWidth/2, displayHeight/2);
	screen2 = std::make_shared<ScreenSnapshot>(displayWidth/2, 0, displayWidth / 2, displayHeight/2);

	checkersEffect = std::make_shared<EffectCheckers>(1, gAppSurface->getBounds(),8, screen1,screen2);
	//paramShard = std::make_shared<ParametricShard>(screenSamp, PixelRect(100,100,640,480), gAppSurface->getBounds());
	
	blankEffect = std::make_shared<VisualEffect>(1,gAppSurface->getBounds(),nullptr, nullptr);
	fadeFromBlack = std::make_shared<CrossFadeEffect>(2, gAppSurface->getBounds(), blankEffect, screen1);
	fadeToBlack = std::make_shared<CrossFadeEffect>(2, gAppSurface->getBounds(), screen1, blankEffect);
	fadeScreen1ToScreen2 = std::make_shared<CrossFadeEffect>(2, gAppSurface->getBounds(), screen1, screen2);
	barnDoorOpen = std::make_shared<BarnDoorOpenEffect>(2, gAppSurface->getBounds(), screen1, screen2);
	barnDoorClose = std::make_shared<BarnDoorCloseEffect>(2, gAppSurface->getBounds(), screen1, screen2);

	currentEffect = blankEffect;

	// setup the recorder
	reco = std::make_shared<Recorder>(&(*gAppSurface), "frame-", 0);


}