#include "gui.h"

#include "qoi.h"
#include "screensnapshot.h"
#include "draw2d.h"

static const int captureWidth=1920;
static const int captureHeight=1080;

// This is the buffer that will be used to encode images
static const size_t bigbuffSize = captureWidth * captureHeight * 4;
byte bigbuff[bigbuffSize];
BinStream bs(bigbuff, bigbuffSize);

ScreenSnapshot snapper(0,0,captureWidth, captureHeight);
User32PixelMap copyMap(captureWidth, captureHeight);

void onFrame()
{
    snapper.next();

	// encode it
	bs.seek(0);
	QOICodec::encode(bs, snapper);

	// decode into copyMap
	bs.seek(0);
	QOICodec::decode(bs, copyMap);

	// Display it
	blit(*gAppSurface, 0, 0, copyMap);

}

void setup()
{
	setCanvasSize(captureWidth, captureHeight);
	setFrameRate(30);
}