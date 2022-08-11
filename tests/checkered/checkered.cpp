#include "gui.h"

#include "checkerboard.h"
#include "targa.h"
#include "imagesampler.h"
#include "screensnapshot.h"
#include "sampledraw2d.h"

#include <memory>

User32PixelMap img;
User32PixelMap img2;

std::shared_ptr<ImageSampler> isamp1 = nullptr;
std::shared_ptr<ImageSampler> isamp2 = nullptr;
std::shared_ptr<ScreenSnapshot> screenSamp = nullptr;
//ScreenSnapshot screenSamp;

auto transSampler = std::make_shared< SolidColorSampler>(0x0);
//auto s1 = std::make_shared< SolidColorSampler>(0xff555555);
//auto s2 = std::make_shared< SolidColorSampler>(0xffdddddd);
//auto red1 = std::make_shared< SolidColorSampler>(0xffff0000);
std::shared_ptr<CheckerSampler> chksamp = nullptr;

void onFrame()
{
    background(PixelRGBA(0));

    screenSamp->next();
    sampleRectangle(*gAppSurface, gAppSurface->frame(), *chksamp);
}

void setup()
{
	setCanvasSize(1280, 1024);

    // Load a Targa image 
    targa::initFromFile(img, "..\\debug\\shuttle.tga");
	//isamp1 = std::make_shared<ImageSampler>(img);
    //screenSamp = std::make_shared<ScreenSnapshot>(320,400,640,480);
    screenSamp = std::make_shared<ScreenSnapshot>(0,0,800,600);


    //targa::initFromFile(img2, "..\\debug\\money-256.tga");
	//isamp2 = std::make_shared<ImageSampler>(img2);

    chksamp = std::make_shared<CheckerSampler>(4, screenSamp, transSampler);
}

// Press Esc to quit the program
void keyReleased(const KeyboardEvent& e)
{
    if (e.keyCode == VK_ESCAPE)
        halt();
}