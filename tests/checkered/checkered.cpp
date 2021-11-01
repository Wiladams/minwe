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

void onFrame()
{
    // larger board with smaller squares

    // color values look like
    // 0xAARRGGBB

    auto s1 = std::make_shared< SolidColorSampler2D>(0xff555555);
    auto s2 = std::make_shared< SolidColorSampler2D>(0xffdddddd);

    // smaller board with larger squares
    screenSamp->next();
    Checkerboard chkfg({ (canvasWidth / 2) - (canvasWidth / 4),
        (canvasHeight / 2) - (canvasHeight / 4),canvasWidth / 2,canvasHeight / 2 },
        s1,
        screenSamp,
        64);
    chkfg.draw(gAppSurface);
}

void setup()
{
	setCanvasSize(1280, 1280);

    // Load a Targa image 
    targa::initFromFile(img, "..\\debug\\shuttle.tga");
	isamp1 = std::make_shared<ImageSampler>(img);
    screenSamp = std::make_shared<ScreenSnapshot>(320,400,640,480);


    targa::initFromFile(img2, "..\\debug\\money-256.tga");
	isamp2 = std::make_shared<ImageSampler>(img2);

    auto red1 = std::make_shared< SolidColorSampler2D>(0xffff0000);
    Checkerboard chkbg({ 0,0,canvasWidth,canvasHeight }, 
        red1, 
        isamp2, 
        canvasWidth/6);
    chkbg.draw(gAppSurface);


}

void keyReleased(const KeyboardEvent& e)
{
    if (e.keyCode == VK_ESCAPE)
        halt();
}