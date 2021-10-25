#include "gui.h"

#include "checkerboard.h"

void setup()
{
	setCanvasSize(640, 640);

    Checkerboard chkbg({ 0,0,canvasWidth,canvasHeight }, 
        0xff555555, 0xffdddddd, 64);
    chkbg.draw(gAppSurface);

    Checkerboard chkfg({ (canvasWidth/2)-(canvasWidth/4),
        (canvasHeight/2)-(canvasHeight/4),canvasWidth/2,canvasHeight/2 }, 
        0x1ccccccc, 0xff0c0c0c, 8);
    chkfg.draw(gAppSurface);
}