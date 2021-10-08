#include "gui.h"
#include "stopwatch.h"



static bool gIsFullscreen = false;

static VOIDROUTINE gSetupHandler = nullptr;
static VOIDROUTINE gDrawHandler = nullptr;

// Keyboard event handling
static KeyEventHandler gKeyPressedHandler = nullptr;
static KeyEventHandler gKeyReleasedHandler = nullptr;
static KeyEventHandler gKeyTypedHandler = nullptr;

static MouseEventHandler gMouseMovedHandler = nullptr;
static MouseEventHandler gMouseClickedHandler = nullptr;
static MouseEventHandler gMousePressedHandler = nullptr;
static MouseEventHandler gMouseReleasedHandler = nullptr;
static MouseEventHandler gMouseWheelHandler = nullptr;
static MouseEventHandler gMouseDraggedHandler = nullptr;



void handleKeyboardEvent(const KeyboardEventTopic& p, const KeyboardEvent& e)
{
    //std::cout << "keyboardEvent: " << e.activity << "\n" ;

    switch (e.activity) {


    case KEYPRESSED:
        //p5::keyCode = e.keyCode;
        if (gKeyPressedHandler) {
            gKeyPressedHandler(e);
        }
        break;
    case KEYRELEASED:
        //p5::keyCode = e.keyCode;
        if (gKeyReleasedHandler) {
            gKeyReleasedHandler(e);
        }
        break;
    case KEYTYPED:
        //p5::keyChar = e.keyCode;
        if (gKeyTypedHandler) {
            gKeyTypedHandler(e);
        }
        break;
    }
}


void handleMouseEvent(const MouseEventTopic& p, const MouseEvent& e)
{

    // If there is a window manager, let it have first crack
    // at the mouse event.
    //if (nullptr != gWindowManager) {
    //    gWindowManager->mouseEvent(e);
    //}

    // If the user has implemented explicit mouse handling routines
    // send the event there.
    switch (e.activity) {


    case MOUSEMOVED:

        if (gMouseMovedHandler != nullptr) {
            gMouseMovedHandler(e);
        }

        //if (p5::mouseIsPressed && gMouseDraggedHandler) {
        //    gMouseDraggedHandler(e);
        //}
        break;

    case MOUSEPRESSED:
        if (gMousePressedHandler != nullptr) {
            gMousePressedHandler(e);
        }
        break;
    case MOUSERELEASED:
        if (gMouseReleasedHandler != nullptr) {
            gMouseReleasedHandler(e);
        }
        if (gMouseClickedHandler != nullptr) {
            gMouseClickedHandler(e);
        }
        break;
    case MOUSEWHEEL:
        //p5::mouseDelta = e.delta;
        if (gMouseWheelHandler != nullptr) {
            gMouseWheelHandler(e);
        }
        break;
    }


}


void fullscreen() noexcept
{

    setCanvasSize(displayWidth, displayHeight);
    setWindowPosition(0, 0);
    layered();
    gIsFullscreen = true;
}

bool isFullscreen() noexcept
{
    return gIsFullscreen;
}

void clearScreenTo(PixelRGBA c)
{
    gAppSurface->setAllPixels(c);
}

uint64_t fFrameRate = 15;
uint64_t fInterval=1000;
double fNextMillis=0;
uint64_t fDroppedFrames=0;
uint64_t frameCount = 0;

StopWatch fsw;

void setFrameRate(const int rate)
{
    fFrameRate = rate;
    fInterval = 1000/rate;
    fNextMillis = fsw.millis() + fInterval;
}

// we get called every time through the main app loop, so 
// we want to deal with frame timing here.
// if we're just past the frame time, then call the 'draw()'
// function if there is one.
// Otherwise, just let the loop continue
// 
// Might also be interesting to check keyboard, mouse, and joystick 
// positions here?
//
void onLoop()
{
    // We'll wait here until it's time to 
    // signal the frame
    if (fsw.millis() > fNextMillis)
    {
        frameCount += 1;
        if (gDrawHandler != nullptr) {
            gDrawHandler();
        }

        // catch up to next frame interval
        while (fNextMillis <= fsw.millis())
        {
            fNextMillis += fInterval;
        }
    }

}

void onLoad()
{
    HMODULE hInst = ::GetModuleHandleA(NULL);
    setFrameRate(15);

    // Look for implementation of keyboard events
    gKeyPressedHandler = (KeyEventHandler)GetProcAddress(hInst, "keyPressed");
    gKeyReleasedHandler = (KeyEventHandler)GetProcAddress(hInst, "keyReleased");
    gKeyTypedHandler = (KeyEventHandler)GetProcAddress(hInst, "keyTyped");

    // Look for implementation of mouse events
    gMouseMovedHandler = (MouseEventHandler)GetProcAddress(hInst, "mouseMoved");
    gMouseClickedHandler = (MouseEventHandler)GetProcAddress(hInst, "mouseClicked");
    gMousePressedHandler = (MouseEventHandler)GetProcAddress(hInst, "mousePressed");
    gMouseReleasedHandler = (MouseEventHandler)GetProcAddress(hInst, "mouseReleased");
    gMouseWheelHandler = (MouseEventHandler)GetProcAddress(hInst, "mouseWheel");
    gMouseDraggedHandler = (MouseEventHandler)GetProcAddress(hInst, "mouseDragged");



    gSetupHandler = (VOIDROUTINE)GetProcAddress(hInst, "setup");
    gDrawHandler = (VOIDROUTINE)GetProcAddress(hInst, "onFrame");

    subscribe(handleKeyboardEvent);
    subscribe(handleMouseEvent);

    // Call a setup routine if the user specified one
    if (gSetupHandler != nullptr) {
        gSetupHandler();
    }


}

