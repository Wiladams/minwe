#include "apphost.h"
#include <stdio.h>

// This little app demonstrates most of the key
// features of the minwe runtime environment

// Do what's necessary before window opens
// Typically do window sizing, and setup 
// subscriptions for events
void onLoad()
{
	puts("onLoad()  :-)");

	
}

// Do whatever cleanup is necessary after window closes
void onUnload()
{
	puts("onUnload() :-(");
}


// Do something every time we check for events
// If this is an animation application, changing the image
// and displaying it typically happens here.
void onLoop()
{
	static int count = 0;
	count = count + 1;
	//printf("Loop: %d\n", count);

	gAppSurface->set(count % 640, 240, 0xffffffff);
	screenRefresh();
}