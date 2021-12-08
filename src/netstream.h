#pragma once

#include "stopwatch.h"
#include "network.h"
#include "binstream.h"

// A class that connects to a network, and
// has streaming capabilities
class NetStream : public IReadBinary
{
	ASocket fSocket;

	size_t fMaxWritten;			// last byte actually written into buffer

	StopWatch fReadTimer;		// Keeps track of last time read occured
	double fReadTimeout;

	StopWatch fWriteTimer;		// Keeps track of last time write occured
	double fWriteTimeout;

	uint64_t fConsumed = 0;
	char LineBuffer[2048];

	int fReadingBufferSize = 2048;
	uint8_t fReadingBuffer[2048];
	BinStream ReadingBuffer;

public:
	NetStream()
		: fMaxWritten(0)
	{
		ReadingBuffer.setData(fReadingBuffer, sizeof(fReadingBuffer));
	}

	NetStream(ASocket s)
		:fSocket(s)
		, fMaxWritten(0)
	{
		ReadingBuffer.setData(fReadingBuffer, sizeof(fReadingBuffer));
	}

	//
	// Dealing with timeouts
	//
	void resetTimeout()
	{
		fReadTimer.reset(0.0);
		fWriteTimer.reset(0.0);
	}

	void setIdleInterval(double seconds)
	{
		setReadTimeout(seconds);
		setWriteTimeout(seconds);
	}

	void setReadTimeout(double seconds)
	{
		fReadTimeout = seconds;
	}

	void setWriteTimeout(double seconds)
	{
		fWriteTimeout = seconds;
	}

	uint64_t getPosition()
	{
		return fConsumed;
	}

	size_t bytesReadyToBeRead() { return fMaxWritten - ReadingBuffer.tell(); }
	size_t advanceWritten(size_t pos)
	{
		if (pos > fMaxWritten)
			fMaxWritten = pos;

		return fMaxWritten;
	}

	void setMaxWritten(size_t written)
	{
		fMaxWritten = written;
	}

	// Streaming read and write
	//
	// refillReadingBuffer
	// We use an internal buffer to read octets from
	// when it runs low, it needs to be refilled
	//
	std::tuple<int, size_t> refillReadingBuffer()
	{
		if (bytesReadyToBeRead() > 0) {
			return std::make_tuple(0, bytesReadyToBeRead());
		}

		// if there are no bytres ready to be read, then 
		// refill our internal buffer from the source

		auto [error, bytesRead] = fSocket.recv((char *)fReadingBuffer, fReadingBufferSize, 0);

		// should check 'error' to see if EOF
		if (bytesRead < 1) {
			return std::make_tuple(error, 0);
		}

		// reset position to beginning
		// and set the maximum number of bytes written
		ReadingBuffer.seek(0);
		setMaxWritten(bytesRead);
	}

	virtual std::tuple<int, uint8_t> readOctet()
	{
		fReadTimer.reset();

		// see if there's something available in the internal
		// buffer that we can return.
		if (bytesReadyToBeRead() > 0)
		{
			return ReadingBuffer.readOctet();
		}

		// If we did not have a byte available, then try
		// to refill the ReadingBuffer
		auto [error, bytesRead] = refillReadingBuffer();

		if (bytesRead > 0)
		{
			auto [error, value] = ReadingBuffer.readOctet();
			return std::make_tuple(error, value);
		}

		// could not read a byte from freshly filled buffer
		// so return error
		return std::make_tuple(-1, 0);
	}

	// Read a single line from the stream
	std::tuple<int, int> readOneLine(char* buffer, size_t size)
	{
		int nchars = 0;
		int error = 0;

		while (nchars < size)
		{
			auto [err, byteRead] = readOctet();
			error = err;
			if (err) {
				break;
			}
			else
			{
				if (byteRead == '\n')
					// if we see an LF, then just break out of the loop
					break;
				else if(byteRead == '\r') {
					// do nothing, just swallow it
					// don't put it in the line
				}
				else {
					buffer[nchars] = byteRead;
					nchars = nchars + 1;
				}

			}
		}

		if (error != 0) // && (error != EOF) 
		{
			return std::make_tuple(error, 0);
		}

		// No interesting errors, so retul size read
		buffer[nchars] = 0;
		return std::make_tuple(0, nchars);
	}
};