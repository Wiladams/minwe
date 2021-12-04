//
// screenserve
//
// Demonstrate the usage of the grayscale 2D sampler.
// This sampler wraps any other source sampler, and converts
// the color values to grayscale values.
//
#include "apphost.h"

#include "screensnapshot.h"
#include "qoi.h"
#include "binstream.h"
#include "tcpserver.h"

#include <memory>

static const int captureWidth = 1920;
static const int captureHeight = 1080;
static const int channels = 4;

// This is the buffer that will be used to encode images
constexpr size_t bigbuffSize = captureWidth * captureHeight * (channels + 1) + sizeof(qoi_header_t) + 4;

byte bigbuff[bigbuffSize];
BinStream bs(bigbuff, bigbuffSize);

// Make the screen snapshot thing
ScreenSnapshot snapper(0,0,captureWidth, captureHeight);

bool sendChunk(IPSocket& s, BufferChunk& bc)
{
	// The ideal size of chunk to send should
	// match the send buffer size of the underlying
	// socket
	static const int idealSize = 16 * 1024;

	// Get a stream on the chunk
	BinStream chunkStream(bc.data(), bc.size());

	uint32_t overallSize = bc.size();

	// write out the overall size to be sent
	int sentCode = s.send((const char*)&overallSize, 4);

	while (!chunkStream.isEOF()) {
		// we'll write idealSize bytes at a time
		// start by writing the number of bytes
		// into the packet header
		int remains = chunkStream.remaining();
		int payloadSize = (int)maths::Min((double)idealSize, (double)remains);

		// Write the payload out and advance
		int bytesSent = s.send((char*)chunkStream.getPositionPointer(), payloadSize);
		overallSize -= bytesSent;

		chunkStream.skip(bytesSent);
	}

	return true;
	
}

void onLoad()
{
	TcpServer srvr(8081);

	if (!srvr.isValid()) {
		// Could not create socket
		printf("Could not create server: %d\n", srvr.getLastError());
		halt();
		return;
	}

	while (true) {
		// Accept a single client
		IPSocket clientSock = srvr.accept(); // wait for a connection

		if (!clientSock.isValid()) {
			printf("clientSock, not valid: %d\n", srvr.getLastError());
			halt();
			return;
		}

		// while connected
		uint64_t commandCount = 0;
		while (true) {
			// Client sends us a command, so read that
			memset(bigbuff, 0, 512);
			int inLen = clientSock.receive((char*)bigbuff, 512);
			commandCount = commandCount + 1;

			if (inLen < 0) {
				printf("TCPReceived ERROR: %d\n", WSAGetLastError());
				//halt();
				break;
			}
			else {
				bigbuff[inLen] = 0;
			}

			// take a snapshot
			snapper.next();

			bs.seek(0);

			QOICodec::encode(bs, snapper);

			size_t outLength = bs.tell();

			// create binstream on outbuff
			BufferChunk bc(bigbuff, outLength);
			sendChunk(clientSock, bc);
		}
	}
}
