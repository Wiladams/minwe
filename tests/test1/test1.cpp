// This little app demonstrates most of the key
// features of the minwe runtime.

#include "apphost.h"
#include <stdio.h>


// a routine to do some simple drawing
void doSomeDrawing()
{
	// clear buffer to white to start
	gAppSurface->clearToPixel(rgb(255, 255, 255));

	// draw a few horizontal lines
	gAppSurface->hline(10, 100, 320, rgb(255, 0, 0));
	gAppSurface->hline(10, 101, 320, rgb(255, 0, 0));

	gAppSurface->hline(10, 102, 320, rgb(0, 255, 0));
	gAppSurface->hline(10, 103, 320, rgb(0, 255, 0));

	gAppSurface->hline(10, 104, 320, rgb(0, 0, 255));
	gAppSurface->hline(10, 105, 320, rgb(0, 0, 255));

	// draw a little rectangle
	gAppSurface->rect(10, 110, 320, 200, rgb(255, 255, 0));

	// make sure to display contents
	screenRefresh();
}


static void mouseSubscriber(const MouseEventTopic& p, const MouseEvent& e)
{
	printf("mouse: %d %d : 0x%x\n",e.x, e.y, e.activity);
}

static void joySubscriber(const JoystickEventTopic& p, const JoystickEvent& e)
{
	printf("joy: %3.2f %3.2f %3.2f\n", e.x, e.y, e.z);
}

// Do what's necessary before window opens
// Typically do window sizing, and setup 
// subscriptions for events
void onLoad()
{
	puts("onLoad()  :-)");

	// Generate all raw input
	//rawInput();



	// register a mouse event handler
	//subscribe(mouseSubscriber);
	//rawMouse();


	subscribe(joySubscriber);
	//legacyJoystick();
	rawJoystick();

	// try to checkout the raw joystick information
	UINT numDevices = 0;
	PRAWINPUTDEVICE pRawInputDevices = nullptr;

	auto res = GetRegisteredRawInputDevices(pRawInputDevices, &numDevices,sizeof(RAWINPUTDEVICE));
	std::vector<uint8_t> buff(numDevices*sizeof(RAWINPUTDEVICE), 0);

	pRawInputDevices = (PRAWINPUTDEVICE)buff.data();
	res = GetRegisteredRawInputDevices(pRawInputDevices, &numDevices, sizeof(RAWINPUTDEVICE));



	doSomeDrawing();

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
}