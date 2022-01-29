//
// Exercise and develop various kinds of visual effects
//
#include "gui.h"
#include "screensnapshot.h"
#include "recorder.h"
#include "stopwatch.h"
#include "sampledraw2d.h"


#include "effect_barndoor.h"
//#include "effect_circlegrid.h"
//#include "effect_slabs.h"
//#include "effect_checkers.h"
#include "effect_crossfade.h"
#include "effect_corners.h"
#include "effect_fingers.h"
#include "effect_push.h"
#include "effect_rainblocks.h"
#include "effect_wiper.h"

using namespace maths;


StopWatch appClock;


// Source Samplers
std::shared_ptr<ScreenSnapshot> screenCapture = nullptr;
std::shared_ptr< SampledWindow> screenCap1 = nullptr;
std::shared_ptr< SampledWindow> screenCap2 = nullptr;
//std::shared_ptr<SolidColorSampler> transSampler = nullptr;

// Wrapping Samplers
//std::shared_ptr<CheckerSampler> checkSamp = nullptr;
std::shared_ptr<LumaSampler> graySamp = nullptr;
//std::shared_ptr<EffectCheckers> checkersEffect = nullptr;

// Pixel Effects
std::shared_ptr<WindowAnimation> blankEffect = nullptr;
std::shared_ptr<CrossFadeEffect> fadeFromBlack = nullptr;
std::shared_ptr<CrossFadeEffect> fadeToBlack = nullptr;
std::shared_ptr<CrossFadeEffect> fadeScreen1ToScreen2 = nullptr;
std::shared_ptr<CrossFadeEffect> fadeScreen2ToScreen1 = nullptr;

// Multi-Block effects
std::shared_ptr<WindowAnimation> horizontalFingersIn = nullptr;
std::shared_ptr<WindowAnimation> verticalFingersIn = nullptr;
std::shared_ptr<WindowAnimation> rainBlocks = nullptr;

// Push and Slide Effects
std::shared_ptr<BarnDoorOpenEffect> barnDoorOpen = nullptr;
std::shared_ptr<BarnDoorCloseEffect> barnDoorClose = nullptr;
std::shared_ptr<CornersFlyOut> cornersFlyOut = nullptr;
std::shared_ptr<Push> pushLeftToRight = nullptr;
std::shared_ptr<Push> pushFromUpperLeft = nullptr;
std::shared_ptr<Push> pushFromTop = nullptr;
std::shared_ptr<WindowAnimation> wiper = nullptr;

std::shared_ptr<WindowAnimation> currentEffect = nullptr;

std::shared_ptr<Recorder> reco = nullptr;


constexpr int FRAMERATE = 15;
double progress = 0;



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

		// Select from our known effects
	case VK_F1:
		currentEffect = fadeFromBlack;
		currentEffect->start();
		break;

	case VK_F2:
		// flip between screens
		currentEffect = fadeScreen1ToScreen2;
		currentEffect->start();
		break;

	case VK_F3:
		// flip between screens
		currentEffect = fadeScreen2ToScreen1;
		currentEffect->start();
		break;

	case VK_F4:
		currentEffect = barnDoorOpen;
		currentEffect->start();
		break;

	case VK_F5:
		currentEffect = barnDoorClose;
		currentEffect->start();
		break;

	case VK_F6:
		currentEffect = cornersFlyOut;
		currentEffect->start();
		break;

	case VK_F7:
		currentEffect = pushLeftToRight;
		currentEffect->start();
		break;

	case VK_F8:
		currentEffect = pushFromUpperLeft;
		currentEffect->start();
		break;

	case VK_F9:
		currentEffect = pushFromTop;
		currentEffect->start();
		break;


	case VK_F10:
		currentEffect = horizontalFingersIn;
		currentEffect->start();
		break;

	case VK_F11:
		currentEffect = verticalFingersIn;
		currentEffect->start();
		break;
/*
	case VK_F12:
		currentEffect = fadeToBlack;
		currentEffect->start();
		break;
*/

	case 'R':
		currentEffect = rainBlocks;
		currentEffect->start();
		break;
	
	case 'W':
		currentEffect = wiper;
		currentEffect->start();
		break;

	}


}

void keyPressed(const KeyboardEvent& e)
{
	switch (e.keyCode) {
	case VK_UP:
		progress += 0.001;
		progress = maths::Clamp(progress, 0.0, 1.0);
		currentEffect->setProgress(progress);
		break;
	case VK_DOWN:
		progress -= 0.01;
		progress = maths::Clamp(progress, 0.0, 1.0);
		currentEffect->setProgress(progress);
		break;


	}
}

// Just to time frames
double thisTime;
double lastTime;



void onFrame()
{
	// If there's no effect, don't do anything
	// keep whatever was last on the screen;
	if (nullptr == currentEffect)
		return;
	
	//thisTime = appClock.millis();
	//auto diffTime = thisTime - lastTime;
	//printf("Interval: %3.2f  FPS: %3.2f\n", diffTime, (double)frameCount / appClock.seconds());
	//lastTime = thisTime;

	screenCapture->next();


	// start with blank slate
	background(PixelRGBA(0));

	// Either call update(), or setProgress()
	// update will be based on the clock within the effect
	//// setProgress() allows you to control the progress externally
	currentEffect->update();

	
	sampleRectangle(*gAppSurface, PixelRect(0, 0, canvasWidth, canvasHeight), *currentEffect);


	reco->saveFrame();
}

void setup()
{
	setCanvasSize(displayWidth/2, displayHeight/2);
	setFrameRate(FRAMERATE);

	// Setup screen captures
	screenCapture = std::make_shared<ScreenSnapshot>(0, 0, displayWidth, displayHeight / 2);
	screenCap1 = std::make_shared<SampledWindow>(screenCapture,TexelRect(0, 0, 0.499, 1.0));
	screenCap2 = std::make_shared<SampledWindow>(screenCapture, TexelRect(0.50, 0, 1.0, 1.0));


	blankEffect = std::make_shared<WindowAnimation>(1);
	
	// dissolve
	fadeFromBlack = std::make_shared<CrossFadeEffect>(2, blankEffect, screenCap1);
	fadeToBlack = std::make_shared<CrossFadeEffect>(2, screenCap2, blankEffect);
	fadeScreen1ToScreen2 = std::make_shared<CrossFadeEffect>(1, screenCap1, screenCap2);
	fadeScreen2ToScreen1 = std::make_shared<CrossFadeEffect>(1, screenCap2, screenCap1);
	
	// Covering Fingers
	horizontalFingersIn = createHFingersIn(1, 32, screenCap1, screenCap2);
	verticalFingersIn = createVFingersIn(1, 64, screenCap1, screenCap2);

	// Barn Doors
	barnDoorOpen = std::make_shared<BarnDoorOpenEffect>(1, screenCap1, screenCap2);
	barnDoorClose = std::make_shared<BarnDoorCloseEffect>(1, screenCap1, screenCap2);

	// Flying pieces
	cornersFlyOut = std::make_shared<CornersFlyOut>(1, screenCap1, screenCap2);
	cornersFlyOut->setEasing(easing::circIn);

	// Miscellaneous
	rainBlocks = createRainBlocks(4, screenCap1, screenCap2);
	//rainBlocks->setEasing(easing::bounceOut);

	wiper = createWiper(3, screenCap1, screenCap2);

	//checkersEffect = std::make_shared<EffectCheckers>(1, 8, screenCap1, blankEffect);
	//checkSamp = std::make_shared<CheckerSampler>(8, screenCap1, blankEffect);
	
	// Pushes
	pushLeftToRight = std::make_shared<Push>(1, screenCap2, screenCap1);
	pushLeftToRight->setEasing(easing::backIn);

	pushFromUpperLeft = std::make_shared<Push>(1,
		screenCap2, TexelRect(0,0,1,1), TexelRect(0,0,1,1), TexelRect(1,1,2,2),
		screenCap1, TexelRect(0,0,1,1), TexelRect(-1,-1,0,0), TexelRect(0,0,1,1));
	
	pushFromTop = std::make_shared<Push>(1.5,
		screenCap2, TexelRect(0, 0, 1, 1), TexelRect(0, 0, 1, 1), TexelRect(0, 1, 1, 2),
		screenCap1, TexelRect(0, 0, 1, 1), TexelRect(0, -1, 1, 0), TexelRect(0, 0, 1, 1));
	pushFromTop->setEasing(easing::bounceOut);


	currentEffect = rainBlocks;

	// setup the recorder
	reco = std::make_shared<Recorder>(&(*gAppSurface), "frame-", 0);

}