//
// graysampler
// Demonstrate the usage of the grayscale 2D sampler.
// This sampler wraps any other source sampler, and converts
// the color values to grayscale values.
//
#include "apphost.h"

#include "screensnapshot.h"
#include "rlecodec.h"
#include "binstream.h"
#include "tcpserver.h"

#include <memory>



static const int captureWidth = 1024;
static const int captureHeight = 768;

// This is the buffer that will be used to encode images
static const size_t bigbuffSize = captureWidth * captureHeight * 4;
byte bigbuff[bigbuffSize];
BinStream bs(bigbuff, bigbuffSize);

// Make the screen snapshot thing
ScreenSnapshot snapper(0,0,captureWidth, captureHeight);

LuminanceRLE rle;



bool sendChunk(IPSocket& s, BufferChunk& bc)
{
	// Get a stream on the chunk
	BinStream chunkStream(bc.data(), bc.size());
	static const int idealSize = 48 * 1024;

	//printf("sendChunk: (%d)\n", bc.size());
	int packetCount = 0;
	int overallSize = bc.size();

	while (!chunkStream.isEOF()) {
		packetCount = packetCount + 1;

		// we'll write 32K bytes at a time
		// start by writing the number of bytes
		// into the packet header
		int remains = chunkStream.remaining();
		int payloadSize = (int)maths::Min((double)idealSize, (double)remains);


		// Write payload size into packet header
		int sentCode = s.send((const char*)&payloadSize, 4);
		//printf("1.0 sentCode: payloadSize: %d\n", payloadSize);

		// Write the payload out and advance
		int bytesSent = s.send((char*)chunkStream.getPositionPointer(), payloadSize);
		overallSize -= bytesSent;

		chunkStream.skip(bytesSent);

		//printf("2.0 actual: (%d) overall: %d remaining: %d\n", 
		//	bytesSent, overallSize, chunkStream.remaining());

	}

	// Send one more packet of size 0
	char packet[1600];
	BinStream packetStream(packet, 1600);

	packetStream.seek(0);
	packetStream.writeUInt32(0);
	int sentCode = s.send(packet, 4);
	//printf("sent chunk\n");

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
		int inLen = clientSock.receive((char *)bigbuff, 512);
		commandCount = commandCount + 1;

		if (inLen < 0) {
			printf("TCPReceived ERROR: %d\n", WSAGetLastError());
			halt();
			break;
		}
		else {
			bigbuff[inLen] = 0;
			//printf("COMMAND[%Id]: (%d) %s\n", commandCount, inLen, (char*)bigbuff);
		}

		// take a snapshot
		snapper.next();

		bs.seek(0);
		rle.Encode(snapper, bs);
		size_t outLength = bs.tell();


		// Send data chunked
		// create binstream on outbuff
		BufferChunk bc(bigbuff, outLength);
		sendChunk(clientSock, bc);
	}
}
