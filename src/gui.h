#pragma once

//
// gui.h
// apphost.h/appmain.cpp give a reasonable core for a windows
// based program.  It fillows a pub/sub paradigm for events, which
// is pretty simple to use.
//
// gui.h/.cpp gives you a function based interface which which 
// is similar to p5 (processing), or other very simple APIs
// If you want something very simple, where you can just implement
// the functions that you use, include gui.h in your application.
//
#include "apphost.h"
#include "draw2d.h"

#pragma comment (lib, "Synchronization.lib")

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
