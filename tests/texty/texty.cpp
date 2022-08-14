#include "gui.h"

void onFrame()
{
    static int fontSize = 12;
    static int dir = 1;

    background(PixelRGBA(0xffffffff));


    //textFont("Arial", fontSize);
    //textFont("Yu Gothic UI", fontSize);
    //textFont("Vivaldi", fontSize);
    //textFont("Stencil", fontSize);
    //textFont("Ravie", fontSize);
    textFont("Bauhaus 93", fontSize);

    text("The Quick brown fox jumps over the lazy dogs back.", 10, 100);

    if (fontSize >= 128)
        dir = -2;
    
    if (fontSize <= 10)
        dir =2;

    fontSize += dir;
}

void setup()
{
	setCanvasSize(1024, 240);
    setFrameRate(20);


}

void keyReleased(const KeyboardEvent& e) {
    switch (e.keyCode)
    {
    case VK_ESCAPE:
        halt();
        break;

    case 'R':
    {
        recordingToggle();
    }
    break;
    }
}
