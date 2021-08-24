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

void onLoad()
{
    printf("onLoad() - called\n");
}
```

onLoad
void onLoad()

onUnload
void onUnload()


onPaint
void onPaint()

onLoop
void onLoop()


