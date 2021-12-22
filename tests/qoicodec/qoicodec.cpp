#include "gui.h"

#include "qoi.h"
#include "screensnapshot.h"

static const int captureWidth=1280;
static const int captureHeight=1024;

// This is the buffer that will be used to encode images
static const size_t bigbuffSize = captureWidth * captureHeight * 4;
byte bigbuff[bigbuffSize];
BinStream bs(bigbuff, bigbuffSize,0,false);

ScreenSnapshot snapper(0,0,captureWidth, captureHeight);

void encodeFrame(BinStream& bs, PixelMap& pmap)
{
	QOICodec::encode(bs, pmap.getPixelPointer(0,0), pmap.width(), pmap.height(), 4);
}

void decodeFrame(BinStream& bs, PixelMap& pmap)
{
	QOICodec::decode(bs, pmap.getPixelPointer(0, 0), pmap.width(), pmap.height(), 4);
}

void onFrame()
{
    snapper.next();

	// encode it
	bs.seek(0);
	encodeFrame(bs, snapper);

	auto size = bs.tell();


	// decode into copyMap
	bs.seek(0);

	decodeFrame(bs, *gAppSurface);
}

void setup()
{
	setCanvasSize(captureWidth, captureHeight);
	setFrameRate(20);
}