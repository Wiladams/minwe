#pragma once

#include "apphost.h"
#include "draw2d.h"

#pragma comment (lib, "Synchronization.lib")

#ifdef __cplusplus
extern "C" {
#endif

	APP_EXPORT extern uint64_t frameCount;


	APP_EXPORT void setup();
	APP_EXPORT void onFrame();

	APP_EXPORT void fullscreen();
	APP_EXPORT void background(PixelRGBA c);

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

	APP_EXPORT void setFrameRate(const int);
#ifdef __cplusplus
}
#endif
