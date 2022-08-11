#include "gui.h"

#include "stopwatch.h"
#include "recorder.h"

struct PixelFont
{

public:
    HFONT   fFont;
    LOGFONTA lFont;

    PixelFont()
        :fFont(NULL)
        , lFont{}
    {
        fFont = NULL;
    }

    PixelFont(const char* name, int height)
    {
        init(name, height);

        //Logical units are device dependent pixels, so this will create a handle to a logical font that is height pixels tall.
        //The width, when set to 0, will cause the font mapper to choose the closest matching value.
        //The font face name will be 'name'.
        lFont = {
            height,                 // cHeight
            0,                      // cWidth
            0,                      // cEscapement
            0,                      // cOrientation
            FW_DONTCARE,            // cWeight
            FALSE,                  // bItalic
            FALSE,                  // bUnderline
            FALSE,                  // bStrikeOut
            DEFAULT_CHARSET,        // iCharset
            OUT_OUTLINE_PRECIS,     // iOutPrecision
            CLIP_DEFAULT_PRECIS,    // iClipPrecision
            CLEARTYPE_QUALITY,      // iQuality
            VARIABLE_PITCH          // iPitchAndFamily
        };
        // faceName
        strncpy_s(lFont.lfFaceName, LF_FACESIZE, name, strlen(name));

        fFont = ::CreateFontIndirectA(&lFont);
    }

    ~PixelFont()
    {
        //reset();
    }

    INLINE void init(const char* name, int height)
    {
        // first clearout whatever is there already
        reset();

        //Logical units are device dependent pixels, so this will create a handle to a logical font that is height pixels tall.
//The width, when set to 0, will cause the font mapper to choose the closest matching value.
//The font face name will be 'name'.
        lFont = {
            height,                 // cHeight
            0,                      // cWidth
            0,                      // cEscapement
            0,                      // cOrientation
            FW_DONTCARE,            // cWeight
            FALSE,                  // bItalic
            FALSE,                  // bUnderline
            FALSE,                  // bStrikeOut
            DEFAULT_CHARSET,        // iCharset
            OUT_OUTLINE_PRECIS,     // iOutPrecision
            CLIP_DEFAULT_PRECIS,    // iClipPrecision
            CLEARTYPE_QUALITY,      // iQuality
            VARIABLE_PITCH          // iPitchAndFamily
        };

        strncpy_s(lFont.lfFaceName, LF_FACESIZE, name, strlen(name));

        fFont = ::CreateFontIndirectA(&lFont);
    }

    INLINE void reset()
    {
        ::DeleteObject(fFont);
    }
};


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
static MouseEventHandler gMouseHWheelHandler = nullptr;
static MouseEventHandler gMouseDraggedHandler = nullptr;

uint64_t fFrameRate = 15;
uint64_t fInterval = 1000;
double fNextMillis = 0;
uint64_t fDroppedFrames = 0;
uint64_t frameCount;


PixelRGBA* pixels = nullptr;

// Keyboard Globals
int keyCode=0;
int keyChar=0;

// Mouse Globals
bool mouseIsPressed = false;
int mouseX=0;
int mouseY=0;
int mouseDelta=0;

int pmouseX=0;
int pmouseY=0;

StopWatch fsw;

Recorder gRecorder;

void recordingStart() { gRecorder.record(); }
void recordingStop() { gRecorder.stop(); }
void recordingPause() { gRecorder.pause(); }
void recordingToggle() { gRecorder.toggleRecording(); }

PixelFont gCurrentFont;     // object for currently selected font



void textFont(const char* fName, int size)
{
    gCurrentFont.init(fName, size);
    ::SelectObject(gAppSurface->getDC(), gCurrentFont.fFont);
}

void text(const char* txt, double x, double y)
{
    ::TextOutA(gAppSurface->getDC(), x, y, txt, strlen(txt));
}

// setting colors
// COLORREF - 0x00bbggrr
//
// PixelRGBA - 0xaarrggbb
//
// When you need a COLORREF to be compatible with 
// some Windows GDI routines
//#define toCOLORREF(c) (c.r() | (c.g() << 8) | (c.b() << 16))

void textColor(const PixelRGBA& pix)
{
    auto c = pix.toCOLORREF();
    ::SetTextColor(gAppSurface->getDC(), c);
}


void handleKeyboardEvent(const KeyboardEventTopic& p, const KeyboardEvent& e)
{
    //std::cout << "keyboardEvent: " << e.activity << "\n" ;
    keyCode = e.keyCode;

    switch (e.activity) 
    {
    case KEYPRESSED:
        if (gKeyPressedHandler) {
            gKeyPressedHandler(e);
        }
        break;
    case KEYRELEASED:
        if (gKeyReleasedHandler) {
            gKeyReleasedHandler(e);
        }
        break;
    case KEYTYPED:
        keyChar = e.keyCode;
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
    pmouseX = mouseX;
    pmouseY = mouseY;
    mouseX = e.x;
    mouseY = e.y;

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
        mouseIsPressed = true;
        if (gMousePressedHandler != nullptr) {
            gMousePressedHandler(e);
        }
        break;

    case MOUSERELEASED:
        mouseIsPressed = false;
        if (gMouseReleasedHandler != nullptr) {
            gMouseReleasedHandler(e);
        }
        if (gMouseClickedHandler != nullptr) {
            gMouseClickedHandler(e);
        }
        break;

    case MOUSEWHEEL:
        if (gMouseWheelHandler != nullptr) {
            gMouseWheelHandler(e);
        }
        break;

    case MOUSEHWHEEL:
        if (gMouseHWheelHandler != nullptr) {
            gMouseHWheelHandler(e);
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

void background(const PixelRGBA &c) noexcept
{
    gAppSurface->setAllPixels(c);
}



void setFrameRate(const int rate)
{
    fFrameRate = rate;
    fInterval = 1000/rate;
    fNextMillis = fsw.millis() + fInterval;
}

// Called by the app framework
// This will be called every time through the main app loop, 
// after the app framework has done whatever processing it 
// needs to do.
//
// We deal with frame timing here because it's the only place
// we have absolute control of what happens in the user's app
//
// if we're just past the frame time, then call the 'draw()'
// function if there is one.
// Otherwise, just let the loop continue
// 

void onLoop()
{
    // We'll wait here until it's time to 
    // signal the frame
    if (fsw.millis() > fNextMillis)
    {
        // WAA - Might also be interesting to get absolute keyboard, mouse, 
        // and joystick positions here.
        //

        frameCount += 1;
        if (gDrawHandler != nullptr) {
            gDrawHandler();
        }
        
        gRecorder.saveFrame();

        // Since we're on the clock, we will refresh
        // the screen.
        refreshScreen();
        
        // catch up to next frame interval
        // this will possibly result in dropped
        // frames, but it will ensure we keep up
        // to speed with the wall clock
        while (fNextMillis <= fsw.millis())
        {
            fNextMillis += fInterval;
        }
    }

}

// Called by the app framework as the first thing
// that happens after the app framework has set itself
// up.  We want to do whatever registrations are required
// for the user's app to run inside here.
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
    gMouseHWheelHandler = (MouseEventHandler)GetProcAddress(hInst, "mouseHWheel");
    gMouseDraggedHandler = (MouseEventHandler)GetProcAddress(hInst, "mouseDragged");



    gSetupHandler = (VOIDROUTINE)GetProcAddress(hInst, "setup");
    gDrawHandler = (VOIDROUTINE)GetProcAddress(hInst, "onFrame");

    subscribe(handleKeyboardEvent);
    subscribe(handleMouseEvent);

    // Start with a default background before setup
    // does something.
    background(PixelRGBA(0xffffffff));

    // setup font
    textFont("Segoe UI", 18);

    // Call a setup routine if the user specified one
    if (gSetupHandler != nullptr) {
        gSetupHandler();
    }

    // setup the recorder
    gRecorder.init(&*gAppSurface);

    // 
    // If there was any drawing done during setup
    // display that at least once.
    refreshScreen();
}

