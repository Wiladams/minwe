#include "gui.h"

#include "checkerboard.h"
#include "targa.h"
#include "imagesampler.h"

#include <memory>


void setup()
{
	setCanvasSize(1280, 1280);

    // Load a Targa image 
    User32PixelMap img;
    targa::initFromFile(img, "d:\\repos\\minwe\\tests\\debug\\shuttle.tga");
	auto isamp1 = std::make_shared<ImageSampler>(img);
	
    User32PixelMap img2;
    targa::initFromFile(img2, "d:\\repos\\minwe\\tests\\debug\\money-256.tga");
	auto isamp2 = std::make_shared<ImageSampler>(img2);

    // color values look like
    // 0xAARRGGBB

    // larger board with smaller squares
	auto s1 = std::make_shared< SolidColorSampler>(0xff555555);
	auto s2 = std::make_shared< SolidColorSampler>(0xffdddddd);
	auto red1 = std::make_shared< SolidColorSampler>(0xffff0000);

    Checkerboard chkbg({ 0,0,canvasWidth,canvasHeight }, 
        red1, isamp2, 320);
    chkbg.draw(gAppSurface);

    // smaller board with larger squares
    Checkerboard chkfg({ (canvasWidth/2)-(canvasWidth/4),
        (canvasHeight/2)-(canvasHeight/4),canvasWidth/2,canvasHeight/2 }, 
        s1, s2, 8);
    chkfg.draw(gAppSurface);
}

void keyReleased(const KeyboardEvent& e)
{
    if (e.keyCode == VK_ESCAPE)
        halt();
}