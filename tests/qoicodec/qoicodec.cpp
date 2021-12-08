#include "gui.h"

#include "qoi.h"
#include "screensnapshot.h"

static const int captureWidth=1280;
static const int captureHeight=1024;

// This is the buffer that will be used to encode images
static const size_t bigbuffSize = captureWidth * captureHeight * 4;
byte bigbuff[bigbuffSize];
BinStream bs(bigbuff, bigbuffSize);

ScreenSnapshot snapper(0,0,captureWidth, captureHeight);

void onFrame()
{
    snapper.next();

	// encode it
	bs.seek(0);
	QOICodec::encode(bs, snapper);
	auto size = bs.tell();


	// decode into copyMap
	bs.seek(0);
	QOICodec::decode(bs, *gAppSurface);
}

void setup()
{
	setCanvasSize(captureWidth, captureHeight);
	setFrameRate(20);
}