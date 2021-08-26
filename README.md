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



