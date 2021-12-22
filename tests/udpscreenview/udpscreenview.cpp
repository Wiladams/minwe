
#include "gui.h"

#include "qoi.h"
#include "binstream.h"
#include "network.h"
#include "timing.h"

#include <memory>

//static const int captureWidth = 1280;
//static const int captureHeight = 1024;
static const int captureWidth = 800;
static const int captureHeight = 600;

static const int channels = 4;

// This is the buffer that will be used to encode images
constexpr size_t bigbuffSize = captureWidth * captureHeight * (channels + 1) + sizeof(qoi_header_t) + 4;

byte bigbuff[bigbuffSize];
BinStream bs(bigbuff, bigbuffSize);


IPAddress srvrAddress;
ASocket udpSender;

// Receive a chunk of stuff back from the server
// Do this in a loop until the chunk size is 0
// or we've exhausted space in the binstream
bool receiveChunk(ASocket& s, IPAddress &addr, BinStream& pixs)
{
	int packetCount = 0;

	// First step is to receive back a 32-bit uint32
	// this indicates the size of the payload
	int32_t overallSize;
	int32_t chunkSize;

	int res = s.receiveFrom(addr, (char*)&overallSize, 4);
	res = s.receiveFrom(addr, (char*)&chunkSize, 4);

	//printf("receiveChunk: %d\n", res);
	//printf("overallSize: %d\n", overallSize);
	//printf("chunkSize: %d\n", chunkSize);

	// if we had an error in receiving, then we return immediately
	// indicating we did not read anything
	if ((SOCKET_ERROR == res) || (res == 0)) {
		return false;
	}

	int remaining = overallSize;

	// make sure we don't run off the end
	// of the chunk buffer
	while ((remaining > 0) && !pixs.isEOF())
	{
		packetCount = packetCount + 1;

		// Now read the rest of the payload into the 
		// pixs stream
		int res = s.receiveFrom(addr, (char*)pixs.getPositionPointer(), chunkSize);

		if (SOCKET_ERROR ==res)
			break;

		pixs.skip(res);
		remaining -= res;
	}

	return true;
}

void onFrame()
{
	//printf("onFrame() - BEGIN\n");

	if (!udpSender.isValid())
	{
		printf("client socket not valid");
		return;
	}

	// each time through
	// check to see if there's content from the server
	// if there is, then read the image in
	// it would be best to do this in its own thread
	// or have a callback mechanism
	const char* commandBuff = "get frame";
	int commandSize = strlen(commandBuff);

	//char srvrbuff[512];
	//srvrAddress.toString(srvrbuff, 511);
	//printf("onFrame.server address: %s\n", srvrbuff);

	// send the server a command
	int res = udpSender.sendTo(srvrAddress, commandBuff, commandSize);
	//printf("onFrame.sendTo: %d\n", res);

	bs.seek(0);
	if (receiveChunk(udpSender, srvrAddress, bs))
	{
		// Decode the image
		bs.seek(0);

		QOICodec::decode(bs, *gAppSurface);
	}
	else {
		printf("error receiving chunk\n");
		halt();
	}

	//printf("onFrame() - END\n");
}

void setup()
{
	printf("setup\n");

	// initialize the sending socket
	udpSender.init(AF_INET, SOCK_DGRAM, IPPROTO_UDP, false);

	// Get the host address we'll capturing from
	IPHost host("192.168.1.9", "9991", SOCK_DGRAM);
	//IPHost host = IPHost::create("localhost", "9991", , SOCK_DGRAM);

	if (host.isValid())
	{
		srvrAddress = host.getAddress(0);
	}
	else {
		printf("Could not connect to host\n");
		halt();
	}

	//if (!client.init(AF_INET, SOCK_DGRAM, IPPROTO_IP, false))
	//{
	//	printf("socket client not valid: %d\n", client.getLastError());
	//	halt();
	//}

	setFrameRate(10);
	setCanvasSize(captureWidth, captureHeight);
}
