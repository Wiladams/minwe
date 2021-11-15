

#include "apphost.h"
#include "draw2d.h"

#include "rlecodec.h"
#include "binstream.h"
#include "tcpclient.h"

#include <memory>


static const int captureWidth = 1024;
static const int captureHeight = 768;

// This is the buffer that will be used to encode images
static const size_t bigbuffSize = captureWidth * captureHeight * 4;
byte bigbuff[bigbuffSize];
BinStream bs(bigbuff, bigbuffSize);

// Recipient PixelMap
User32PixelMap clientMap(captureWidth, captureHeight);
LuminanceRLE rle;

IPHost* host = nullptr;
TcpClient* client = nullptr;

// Receive a chunk of stuff back from the server
// Do this in a loop until the chunk size is 0
// or we've exhausted space in the binstream
bool receiveChunk(TcpClient* s, BinStream& pixs)
{
	int packetCount = 0;

	// make sure we don't run off the end
	// of the chunk buffer
	while (!pixs.isEOF()) {
		packetCount = packetCount + 1;

		// First step is to receive back a 32-bit uint32
		// this indicates the size of the payload
		int payloadSize;
		int recvLen = s->receive((char*)&payloadSize, 4);
		//printf("Message Received: (%d) [%d]  Payload Size: %d\n", packetCount, recvLen, payloadSize);

		// if we had an error in receiving, then we return immediately
		// indicating we did not read anything
		if (recvLen < 0) {
			return false;
		}


		if (payloadSize == 0) {
			// We've reached a zero sized payload
			// we're at the end of message
			// so break out of the loop
			//printf("== EOM ==");
			break;
		}

		// Now read the rest of the payload into the 
		// pixs stream
		recvLen = s->receive((char*)pixs.getPositionPointer(), payloadSize);
		pixs.skip(recvLen);
		//printf("  Payload: (%d) [%d]  Payload Size: %d\n", packetCount, recvLen, payloadSize);
	}

	return true;
}


void onLoop()
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
		//printf("send command: %d\n", err);

		bs.seek(0);
		if (receiveChunk(client, bs))
		{
			// Decode the image
			bs.seek(0);
			rle.Decode(bs, clientMap);
		}
	}

	// Display it
	blit(*gAppSurface, 0, 0, clientMap);


	refreshScreen();
}



void onLoad()
{
	printf("onload\n");

	//host = IPHost::create("192.168.1.9", "8081");
	host = IPHost::create("localhost", "8081");

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

	setCanvasSize(captureWidth, captureHeight);
}
