#include "gui.h"

#include "checkerboard.h"
#include "targa.h"
#include "imagesampler.h"
#include "screensnapshot.h"

#include <memory>

User32PixelMap img;
User32PixelMap img2;

std::shared_ptr<ImageSampler> isamp1 = nullptr;
std::shared_ptr<ImageSampler> isamp2 = nullptr;
std::shared_ptr<ScreenSnapshot> screenSamp = nullptr;

auto s1 = std::make_shared< SolidColorSampler>(0xff555555);
auto s2 = std::make_shared< SolidColorSampler>(0xffdddddd);
auto red1 = std::make_shared< SolidColorSampler>(0xffff0000);

void onFrame()
{
    background(0);

    // Generic background gray checkerboard
    Checkerboard bkgnd({0,0,canvasWidth,canvasHeight}, PixelRGBA(0xffCCCCCC), PixelRGBA(0xff0f0f0f), 8);
    bkgnd.draw(gAppSurface);

    // smaller board with larger squares
    screenSamp->next();
    Checkerboard chkfg({ (canvasWidth / 2) - (canvasWidth / 4),
        (canvasHeight / 2) - (canvasHeight / 4),canvasWidth / 2,canvasHeight / 2 },
        s1,
        screenSamp,
        64);
    //chkfg.draw(gAppSurface);
}

void setup()
{
	setCanvasSize(1280, 1024);

    // Load a Targa image 
    targa::initFromFile(img, "..\\debug\\shuttle.tga");
	isamp1 = std::make_shared<ImageSampler>(img);
    screenSamp = std::make_shared<ScreenSnapshot>(320,400,640,480);

    targa::initFromFile(img2, "..\\debug\\money-256.tga");
	isamp2 = std::make_shared<ImageSampler>(img2);

    Checkerboard chkbg({ 0,0,canvasWidth,canvasHeight }, 
        red1, 
        isamp2, 
        canvasWidth/6);
    //chkbg.draw(gAppSurface);


}

// Press Esc to quit the program
void keyReleased(const KeyboardEvent& e)
{
    if (e.keyCode == VK_ESCAPE)
        halt();
}