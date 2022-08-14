#pragma once

//
// gui.h
// //
// apphost.h/appmain.cpp give a reasonable core for a windows
// based program.  It follows a pub/sub paradigm for events, which
// is pretty simple to use.
//
// gui.h/.cpp gives you a function based interface which which 
// is similar to p5 (processing), or other very simple APIs
// If you want something very simple, where you can just implement
// the functions that you use, include gui.h in your application.
//
#include "apphost.h"
#include "sampledraw2d.h"
#include "recorder.h"

#pragma comment (lib, "Synchronization.lib")

#ifdef __cplusplus
extern "C" {
#endif


	// Application can calls these, they are part of the 
	// gui API
	APP_EXPORT void fullscreen() noexcept;
	APP_EXPORT void background(const PixelRGBA &c) noexcept;
	APP_EXPORT void setFrameRate(const int);

	// Application can call these to handle recording
	APP_EXPORT void recordingStart();
	APP_EXPORT void recordingStop();
	APP_EXPORT void recordingPause();
	APP_EXPORT void recordingToggle();



	// Application can implement these
	// Should at least implement setup(), so canvas size
	// can be set
	APP_EXPORT void setup();

	// If application implements 'onFrame()', it
	// is called based on the frequency of the 
	// frame rate specified
	APP_EXPORT void onFrame();

	// keyboard event processing
	// Application can implement these
	typedef void (*KeyEventHandler)(const KeyboardEvent& e);

	APP_EXPORT void keyPressed(const KeyboardEvent& e);
	APP_EXPORT void keyReleased(const KeyboardEvent& e);
	APP_EXPORT void keyTyped(const KeyboardEvent& e);

	// mouse event processing
	// Application can implement these
	typedef void (*MouseEventHandler)(const MouseEvent& e);

	APP_EXPORT void mouseClicked(const MouseEvent& e);
	APP_EXPORT void mouseDragged(const MouseEvent& e);
	APP_EXPORT void mouseMoved(const MouseEvent& e);
	APP_EXPORT void mousePressed(const MouseEvent& e);
	APP_EXPORT void mouseReleased(const MouseEvent& e);
	APP_EXPORT void mouseWheel(const MouseEvent& e);
	APP_EXPORT void mouseHWheel(const MouseEvent& e);



#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
extern "C" {
#endif
	// These are variables available to the application
	// Size of the application area, set through
	// setCanvasSize()
	APP_EXPORT extern int width;
	APP_EXPORT extern int height;

	APP_EXPORT extern uint64_t frameCount;
	APP_EXPORT extern uint64_t droppedFrames;

	APP_EXPORT extern PixelRGBA* pixels;

	// Keyboard Globals
	APP_EXPORT extern int keyCode;
	APP_EXPORT extern int keyChar;

	// Mouse Globals
	APP_EXPORT extern bool mouseIsPressed;	// a mouse button is currently pressed
	APP_EXPORT extern int mouseX;			// last reported location of mouse
	APP_EXPORT extern int mouseY;			
	APP_EXPORT extern int mouseDelta;		// last known delta of mouse wheel
	APP_EXPORT extern int pmouseX;
	APP_EXPORT extern int pmouseY;

#ifdef __cplusplus
}
#endif
