# minwe
 Minimum Windowing Environment

I have several projects that create a minimum shim to the Windows UI environment as part of their framework.  This projects takes some of the learnings from those various projects and puts the code in one place.

The language is C++.  It's pretty basic C++ though.  I avoid using more exotic features
primarily sticking to what can be found in C++ 2011, if that.

Building
The environment assumes you are building within the context of a standard C++ project
on Windows.  It makes usage of includes and whatnot that should be standard if you're
using C++ or any other compiler environment on Windows.

Philosophy
==========
Since this code has been built up, borrowed, loaned, morphed, rewritten, several
times over the years, I try to do a few things.
Minimum required
Duplicate for convenience
Don't use super fancy stuff

It is an opinionated implementation.  At the very core, there is an eventing
system that is based on a publish/subscribe model.  Consumers of this little library
are meant to subscribe to this system, and construct their own eventing model atop that.

In addition, the framework has its own "main" eventing loop, which allows the app
developer to create their own thing.

How to Use
==========
If you just compile what's in the 'src' directory, you'll end up with a working
program that will open up a blank window on the screen of size 640x480.  The main() 
routine is in the appmain.cpp file.  So, you don't need anything more than that
but, the application won't do much.

This appmain.cpp sets up a minimal runtime environment which is ready for 
you to program.  The runtime environment consists of a few things, and a few
points at which you can interact with it.


If you want to create your own application, create a project of your own, and
include the code that's found in the 'src' directory.  The 'testy' directory
contains several projects that demonstrate how to do this.

The way you interact with the runtime is to simply implement a few key
functions within your own code.  The runtime will look for implementations
of these functions.  If they exist, it will call them at appropriate times.

The first two functions you might want to implement are 'void onLoad()' and
'void onUnload()'.  The first function, onLoad(), is called after the runtime
has set itself up, and is giving you a chance to do something before
the runtime starts its main loop.

The onUnload() function is called once the runtime has exited its main loop
and gives you a chance to do something before the application shuts down.

```C
#include "apphost.h"
#include <stdio.h>

void onLoad()
{
	puts("onLoad()  :-)");
}

void onUnload()
{
	puts("onUnload() :-(");
}
```

With that you have the bare minimum application that can actually do
your bidding.

onLoad
void onLoad()

onUnload
void onUnload()

Messaging
---------
Windows is a message driven system.  Everything that occurs from mouse
movement, to keyboard interaction and drawing, is associated with some
form of message.  A message is simply a chunk of data containing the
particulars, like where the mouse is, which buttons are pressed and so on.

There are many different kinds of messages in the Windows system.  The
challenge of programming with them is knowing all the little esoteric details
of the data structures, their flags and bitfields, and different meanings
under differing circumstances.  minwe simplifies the situation by 
turning the various standard messages into simpler functions that the 
user can implement.

The first of these functions has to do with the 'messaging loop'.  This
so called loop is the top level control mechanism of the application.

The core of the message loop is this

```C
    while (true) {
        // we use peekmessage, so we don't stall on a GetMessage
        // should probably throw a wait here
        // WaitForSingleObject
        BOOL bResult = ::PeekMessageA(&msg, NULL, 0, 0, PM_REMOVE);
        
        if (bResult > 0) {
            // If we see a quit message, it's time to stop the program
            if (msg.message == WM_QUIT) {
                break;
            }

            res = ::TranslateMessage(&msg);
            res = ::DispatchMessageA(&msg);
        }
        else {
            // call onLoop() if it exists
            if (gOnLoopHandler != nullptr) {
                gOnLoopHandler();
            }
        }
    }
```

Basically an infinite loop which queries the Windows system for 
any messages that might be waiting (PeekMessage()).  If there are
any messages waiting, then deal with them (Translate/Dispatch).
Dealing with the messages is the bulk of the rest of the code
in appmain.cpp.

Once those standard messages have been dealt with, or if there
are no messages to deal with, then the user implemented 
onLoop() function is called.  If the user does not implement
a function called onLoop(), then nothing additional will be
implemented, and we go back to checking for standard messages.

onLoop
void onLoop()

Expanding on the previous example:

```C
#include "apphost.h"
#include <stdio.h>

void onLoad()
{
	puts("onLoad()  :-)");
}

void onUnload()
{
	puts("onUnload() :-(");
}

void onLoop()
{
	static int count = 0;
	count = count + 1;
	printf("Loop: %d\n", count);
}
```

At this point, your application knows 
* when it is loaded and ready to run
* when the main event loop is finished and the app is ready to unload
* each time we've gone through the main event loop

The last of these user implementable functions is onPaint().  This message
is very Windows specific, and counter to the rest of the framework, so 
won't be covered yet, and might actually be removed.

onPaint
void onPaint()


So, how to get something up on the screen?

The runtime has some global variables and functions that
are available.  These functions and variables are the ways
in which you interact with the runtime.

```C
// Access to command line arguments
int gargc;
**gargv;

// pointer to the application's window
User32Window * gAppWindow;

// pointer to the app window's frame buffer
std::shared_ptr<User32PixelMap> gAppSurface;

// Miscellaneous useful information
int displayWidth;
int displayHeight;
unsigned int systemDpi;     // System logical dots per inch
unsigned int systemPpi;     // System physical dots per inch

int canvasWidth;            // The width set by the user
int canvasHeight;           // The height set by the user
```

Functions
=========

```C
void showAppWindow();
void halt();

void refreshScreen();

void layered();
void noLayered();
bool isLayered();

void rawInput();
void noRawInput();

void joystick();
void noJoystick();

// Turn on/off touch input
bool touch();
bool noTouch();
bool isTouch();

// Turn on/off file drop handling
bool dropFiles();
bool noDropFiles();

// Turn on/off visual cursor
void cursor();
void noCursor();

// show and hide the application window
void show();
void hide();


// Set the position of the window
void setWindowPosition(int x, int y);

// Set the size of the drawing area of the window
bool setCanvasSize(long aWidth, long aHeight);
```

Publish/Subscribe system
========================
The previous fixed functions are useful for turning features on and
off, and performing simple operations such as window repositioning, 
but for handling the events generated by input devices (mouse, keyboard).

Data from input devices can come at any time as these are asynchronous actions.  We want to turn this data into 'events' as early as possible 
because the details of how to get the data, and how to decode it, is
very esoteric and error prone.  Once we have turned the raw data into
an 'event', we want to distribute that event in such a way that is loosely
coupled.  The Publish/Subscribe pattern is used here.

Before the user applicaton starts (perhaps in the onLoad() implementation),
the app can register a function to be called whenever any of the specific
events is seen in the system.  For simplicity, there are convenience
functions to allow for quick subscription to known event types.

```C
void subscribe(SignalEventTopic::Subscriber s);
void subscribe(MouseEventTopic::Subscriber s);
void subscribe(KeyboardEventTopic::Subscriber s);
void subscribe(JoystickEventTopic::Subscriber s);
void subscribe(FileDropEventTopic::Subscriber s);
void subscribe(TouchEventTopic::Subscriber s);
void subscribe(PointerEventTopic::Subscriber s);
```

You can actually subscribe to the events more than once, to process them in different
parts of the application, but it is typical to only subscribe once and have all the 
processing occur in one place.

A typical subscription might look like this

```C
void handleMouseEvent(const MouseEventTopic& p, const MouseEvent& e)
{
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

    subscribe(handleMouseEvent);
```

By including apphost.h, you can create a fair number of applications, and you are
in complete control of eventing model, drawing, and the like.  There is another
convenience layer.

```C
// gui.h
#ifdef __cplusplus
extern "C" {
#endif

	APP_EXPORT void setup();
	APP_EXPORT void onFrame();

	APP_EXPORT void fullscreen() noexcept;
	APP_EXPORT void background(const PixelRGBA &c) noexcept;

	// keyboard event processing
	typedef void (*KeyEventHandler)(const KeyboardEvent& e);

	APP_EXPORT void keyPressed(const KeyboardEvent& e);
	APP_EXPORT void keyReleased(const KeyboardEvent& e);
	APP_EXPORT void keyTyped(const KeyboardEvent& e);

	// mouse event processing
	typedef void (*MouseEventHandler)(const MouseEvent& e);

	APP_EXPORT void mouseClicked(const MouseEvent& e);
	APP_EXPORT void mouseDragged(const MouseEvent& e);
	APP_EXPORT void mouseMoved(const MouseEvent& e);
	APP_EXPORT void mousePressed(const MouseEvent& e);
	APP_EXPORT void mouseReleased(const MouseEvent& e);
	APP_EXPORT void mouseWheel(const MouseEvent& e);
	APP_EXPORT void mouseHWheel(const MouseEvent& e);

	APP_EXPORT void setFrameRate(const int);
#ifdef __cplusplus
}
#endif

```

These functions are all intended to be implemented by the user's application.
When compiling, simply include gui.cpp as one of the files.  Perhaps the most
important is setup().  This function, if the application implements it, is called
after various libraries and routines have been loaded and initiated, but before
the main event processing loop begins.  It is convenient to implement sizing of 
the main window, and whatever other processing should occur before the application
really gets going.

A typical application might look like this;

```C
#include "gui.h"
#include "sampler.h"

void setup()
{
	setCanvasSize(1024, 768);
    setFrameRate(15);

}
```

In this case, the application drawing canvas will be set to 1024 x 768.  In addition
the frame rate of the application is set to 15 frames per second.

```C
void onFrame()
{
    // Get current screen snapshot
    screenSamp->next();

    // Draw a rectangle with snapshot as texture
    sampleRectangle(*gAppSurface,gAppSurface->frame(),*screenSamp);
}
```

If the application implements the onFrame() function, it will be called every time
through the event loop, based on the frame rate.  This is typically whatever drawing
is to be done occurs.

It is similar with the various mouse and keyboard functions.  If they are implemented,
they will be called at the appropriate times in accordance with the event handler implemented
within gui.cpp.  If the application does not implement these routines, they simply are not called.

gui.h contains a few global variables that can be accessed from within the application.

```C
#ifdef __cplusplus
extern "C" {
#endif
	// Size of the application area, set through
	// createCanvas()
	APP_EXPORT extern int width;
	APP_EXPORT extern int height;

	APP_EXPORT extern uint64_t frameCount;
	APP_EXPORT extern uint64_t droppedFrames;

	APP_EXPORT extern PixelRGBA* pixels;

	// Keyboard Globals
	APP_EXPORT extern int keyCode;
	APP_EXPORT extern int keyChar;

	// Mouse Globals
	APP_EXPORT extern bool mouseIsPressed;
	APP_EXPORT extern int mouseX;
	APP_EXPORT extern int mouseY;
	APP_EXPORT extern int mouseDelta;
	APP_EXPORT extern int pmouseX;
	APP_EXPORT extern int pmouseY;


#ifdef __cplusplus
}
#endif
```
