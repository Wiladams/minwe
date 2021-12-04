
#include "gui.h"

#include "qoi.h"
#include "binstream.h"
#include "tcpclient.h"

#include <memory>

static const int captureWidth = 1920;
static const int captureHeight = 1080;
static const int channels = 4;

// This is the buffer that will be used to encode images
constexpr size_t bigbuffSize = captureWidth * captureHeight * (channels + 1) + sizeof(qoi_header_t) + 4;

byte bigbuff[bigbuffSize];
BinStream bs(bigbuff, bigbuffSize);

// Recipient PixelMap
User32PixelMap clientMap(captureWidth, captureHeight);

IPHost* host = nullptr;
TcpClient* client = nullptr;

// Receive a chunk of stuff back from the server
// Do this in a loop until the chunk size is 0
// or we've exhausted space in the binstream
bool receiveChunk(TcpClient* s, BinStream& pixs)
{
	int packetCount = 0;

	// First step is to receive back a 32-bit uint32
	// this indicates the size of the payload
	int payloadSize;
	int recvLen = s->receive((char*)&payloadSize, 4);
	int remaining = payloadSize;
	
	// if we had an error in receiving, then we return immediately
	// indicating we did not read anything
	if (recvLen < 0) {
		return false;
	}

	// make sure we don't run off the end
	// of the chunk buffer
	while ((remaining>0) && !pixs.isEOF()) 
	{
		packetCount = packetCount + 1;

		// Now read the rest of the payload into the 
		// pixs stream
		recvLen = s->receive((char*)pixs.getPositionPointer(), remaining);

		if (recvLen == SOCKET_ERROR)
			break;

		pixs.skip(recvLen);
		remaining -= recvLen;
	}

	return true;
}

void onFrame()
{
	// each time through
	// check to see if there's content from the server
	// if there is, then read the image in
	// it would be best to do this in its own thread
	// or have a callback mechanism
	if (client != nullptr)
	{
		// send the server a command
		auto err = client->send("get frame", 9);

		bs.seek(0);
		if (receiveChunk(client, bs))
		{
			// Decode the image
			bs.seek(0);
			QOICodec::decode(bs, clientMap);
		}
	}

	// Display it
	blit(*gAppSurface, 0, 0, clientMap);
	//sampleRectangle(*gAppSurface, 0, 0, captureWidth / 2, captureHeight / 2, clientMap);
	//sampleRectangle(*gAppSurface, captureWidth / 2, captureHeight / 2, captureWidth / 2, captureHeight / 2, clientMap);

	refreshScreen();
}

void setup()
{
	printf("setup\n");

	host = IPHost::create("192.168.1.9", "8081");
	//host = IPHost::create("localhost", "8081");

	if (host != nullptr)
	{
		IPAddress* addr = host->getAddress(0);
		client = new TcpClient(addr);
	}

	if (!client->isValid())
	{
		printf("socket client not valid: %d\n", client->getLastError());
		halt();
	}

	setFrameRate(10);
	setCanvasSize(captureWidth, captureHeight);
}
