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

static const int captureWidth = 1280;
static const int captureHeight = 1024;
static const int channels = 4;

// This is the buffer that will be used to encode images
constexpr size_t bigbuffSize = captureWidth * captureHeight * (channels + 1) + sizeof(qoi_header_t) + 4;

byte bigbuff[bigbuffSize];
BinStream bs(bigbuff, bigbuffSize);

// Make the screen snapshot thing
ScreenSnapshot snapper(0,0,captureWidth, captureHeight);

bool sendChunk(ASocket& s, char * buff, int buffLen)
{
	// The ideal size of chunk to send should
	// match the send buffer size of the underlying
	// socket
	//static const int idealSize = 16 * 1024;
	int idealSize = s.getSendBufferSize();

	// Get a stream on the chunk
	BinStream chunkStream(buff, buffLen);

	uint32_t overallSize = buffLen;

	// write out the overall size to be sent
	// if we fail on this return false
	auto [error, bytesSent] = s.send((const char*)&overallSize, 4);
	if (error != 0)
		return false;

	// While there's still stuff to be sent
	// send it out
	while (!chunkStream.isEOF()) {
		// we'll write idealSize bytes at a time
		// start by writing the number of bytes
		// into the packet header
		int remains = chunkStream.remaining();
		int payloadSize = (int)maths::Min((double)idealSize, (double)remains);

		// Write the payload out and advance
		auto [error, bytesSent] = s.send((char*)chunkStream.getPositionPointer(), payloadSize);

		// If there was an error
		// return false
		if (error != 0)
			return false;


		overallSize -= bytesSent;

		chunkStream.skip(bytesSent);
	}

	return true;
	
}

void onLoad()
{
	TcpServer srvr(8081);
	ASocket clientSock;

	if (!srvr.isValid()) {
		// Could not create socket
		printf("Could not create server: %d\n", srvr.getLastError());
		halt();
		return;
	}

	while (true) {
		// Accept a single client
		clientSock = srvr.accept(); // wait for a connection

		if (!clientSock.isValid()) {
			printf("clientSock, not valid: %d\n", srvr.getLastError());
			halt();
			return;
		}

		// while connected
		uint64_t commandCount = 0;
		while (true) {
			// Client sends us a command, so read that
			// command should be 'send frame'
			memset(bigbuff, 0, 512);
			auto [error, inLen] = clientSock.recv((char*)bigbuff, 512);
			commandCount = commandCount + 1;

			if (inLen < 0 || (error != 0)) {
				printf("TCPReceived ERROR: %d\n", WSAGetLastError());

				break;
			}
			else {
				// we got a command
				// we'll assume it's for 
				// taking a snapshot and sending it
				bigbuff[inLen] = 0;
			}

			// take a snapshot
			snapper.next();

			bs.seek(0);

			QOICodec::encode(bs, snapper);

			size_t outLength = bs.tell();

			// create binstream on outbuff
			sendChunk(clientSock, (char *)bigbuff, outLength);
		}

		printf("client left\n");
	}
}
