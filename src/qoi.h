#pragma once

/*

QOI - The “Quite OK Image” format for fast, lossless image compression

Dominic Szablewski - https://phoboslab.org


-- LICENSE: The MIT License(MIT)

Copyright(c) 2021 Dominic Szablewski

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files(the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and / or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions :
The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.


-- About

QOI encodes and decodes images in a lossless format. An encoded QOI image is
usually around 10--30% larger than a decently optimized PNG image.

QOI outperforms simpler PNG encoders in compression ratio and performance. QOI
images are typically 20% smaller than PNGs written with stbi_image but 10%
larger than with libpng. Encoding is 25-50x faster and decoding is 3-4x faster
than stbi_image or libpng.

*/

/*
	Note: William A Adams
	The original functions assumed writing to a file.  I want to work with 
	streams instead, so binstream is used.

	Also, a chunk was allocated for every frame encoded.  I want to change
	this to using a pre-allocated chunk (the stream) so we save on 
	alloc/free for every frame.
*/


#include "binstream.h"


namespace QOICodec
{
#define QOI_INDEX   0x00 // 00xxxxxx
#define QOI_RUN_8   0x40 // 010xxxxx
#define QOI_RUN_16  0x60 // 011xxxxx
#define QOI_DIFF_8  0x80 // 10xxxxxx
#define QOI_DIFF_16 0xc0 // 110xxxxx
#define QOI_DIFF_24 0xe0 // 1110xxxx
#define QOI_COLOR   0xf0 // 1111xxxx

#define QOI_MASK_2  0xc0 // 11000000
#define QOI_MASK_3  0xe0 // 11100000
#define QOI_MASK_4  0xf0 // 11110000



union qoi_rgba_t 
{
	struct { unsigned char r, g, b, a; } rgba;
	unsigned int v;
} ;

union qoi_magic_t 
{
	char chars[4];
	unsigned int v;
} ;

struct qoi_header_t 
{
	qoi_magic_t magic;
	uint32_t width;
	uint32_t height;
	uint8_t channels;
	uint8_t colorspace;
} ;

const static qoi_magic_t qoi_magic = { .chars = {'q','o','i','f'} };

#define QOI_COLOR_HASH(C) (C.rgba.r ^ C.rgba.g ^ C.rgba.b ^ C.rgba.a)


	// return the maximum number of bytes needed to encode the image
	// this can be used to allocate a buffer to encode into
	static int maxBufferSize(const int w, const int h, const int channels) {
		int max_size = w * h * (channels + 1) + sizeof(qoi_header_t) + 4;
		return max_size;
	}


	// Encode raw RGB or RGBA pixels into a QOI image in memory.
	// The function either returns false on failure or true on success
	//
	// WAA - Assume the stream is already capable of handling maxBufferSize() amount
	// of data
	static bool encode(BinStream& bs, void *data, int w, int h, int channels, int colorspace=0) 
	{
		// Quick reject if parameters are not correct
		if (
			data == NULL  ||
			w <= 0 || w >= (1 << 16) ||
			h <= 0 || h >= (1 << 16) ||
			channels < 3 || channels > 4
			) {
			return false;
		}

		// Write the header
		bs.writeBytes(qoi_magic.chars, 4);
		bs.writeUInt32(w);
		bs.writeUInt32(h);
		bs.writeOctet((uint8_t)channels);
		bs.writeOctet((uint8_t)colorspace);

		int dataStart = bs.tell();

		const unsigned char* pixels = (const unsigned char*)data;

		qoi_rgba_t index[64] = { 0 };

		int run = 0;
		qoi_rgba_t px_prev{0, 0, 0, 255};
		qoi_rgba_t px = px_prev;

		int px_len = w * h * channels;
		int px_end = px_len - channels;
		for (int px_pos = 0; px_pos < px_len; px_pos += channels) {
			if (channels == 4) {
				px = *(qoi_rgba_t*)(pixels + px_pos);
			}
			else {
				px.rgba.r = pixels[px_pos];
				px.rgba.g = pixels[px_pos + 1];
				px.rgba.b = pixels[px_pos + 2];
			}

			// check to see if the pixel is the same
			// if it is, increase the run
			if (px.v == px_prev.v) {
				run++;
			}

			if ((run > 0) && (run == 0x2020 || px.v != px_prev.v || px_pos == px_end)) {
				if (run < 33) {
					run -= 1;
					bs.writeOctet(QOI_RUN_8 | run);
				}
				else {
					run -= 33;
					bs.writeOctet(QOI_RUN_16 | (run >> 8));
					bs.writeOctet(run);
				}
				run = 0;
			}

			if (px.v != px_prev.v) {
				int index_pos = QOI_COLOR_HASH(px) % 64;

				if (index[index_pos].v == px.v) {
					bs.writeOctet(QOI_INDEX | index_pos);
				}
				else {
					index[index_pos] = px;

					int vr = (int)px.rgba.r - (int)px_prev.rgba.r;
					int vg = (int)px.rgba.g - (int)px_prev.rgba.g;
					int vb = (int)px.rgba.b - (int)px_prev.rgba.b;
					int va = (int)px.rgba.a - (int)px_prev.rgba.a;

					if (
						vr > -16 && vr < 17 && vg > -16 && vg < 17 &&
						vb > -16 && vb < 17 && va > -16 && va < 17
						) {
						if (
							va == 0 && vr > -2 && vr < 3 &&
							vg > -2 && vg < 3 && vb > -2 && vb < 3
							) {
							bs.writeOctet(QOI_DIFF_8 | ((vr + 1) << 4) | (vg + 1) << 2 | (vb + 1));
						}
						else if (
							va == 0 && vr > -16 && vr < 17 &&
							vg > -8 && vg < 9 && vb > -8 && vb < 9
							) {
							bs.writeOctet(QOI_DIFF_16 | (vr + 15));
							bs.writeOctet(((vg + 7) << 4) | (vb + 7));
						}
						else {
							bs.writeOctet(QOI_DIFF_24 | ((vr + 15) >> 1));
							bs.writeOctet(((vr + 15) << 7) | ((vg + 15) << 2) | ((vb + 15) >> 3));
							bs.writeOctet(((vb + 15) << 5) | (va + 15));
						}
					}
					else {
						bs.writeOctet(QOI_COLOR | (vr ? 8 : 0) | (vg ? 4 : 0) | (vb ? 2 : 0) | (va ? 1 : 0));
						if (vr) { bs.writeOctet(px.rgba.r); }
						if (vg) { bs.writeOctet(px.rgba.g); }
						if (vb) { bs.writeOctet(px.rgba.b); }
						if (va) { bs.writeOctet(px.rgba.a); }
					}
				}
			}
			px_prev = px;
		}

		for (int i = 0; i < 4; i++) {
			bs.writeOctet(0);
		}

		return true;
	}

	// Decode a QOI image from memory into either raw RGB (channels=3) or RGBA 
	// (channels=4) pixel data.
	// The function either returns false on failure (invalid parameters) 
	// or true. 
	static bool decode(BinStream& bs, void* data, int w, int h, int channels)
	{
		if (bs.remaining() < sizeof(qoi_header_t))
		{
			return false;
		}

		// Read in the header
		qoi_header_t header;
		bs.readBytes((uint8_t*)header.magic.chars, 4);
		header.width = bs.readUInt32();
		header.height = bs.readUInt32();
		header.channels = bs.readOctet();
		header.colorspace = bs.readOctet();

		if ((header.channels < 3) ||
			(header.channels > 4) ||
			(header.width != w) ||
			(header.height != h) ||
			(header.magic.v != qoi_magic.v)
			)
		{
			return false;
		}

		// how many bytes are needed to hold pixel data
		int px_len = header.width * header.height * header.channels;


		// We're going to decode directly into the pixel pointer
		// of the destination PixelMap
		unsigned char* pixels = (unsigned char*)data;

		//int data_len = header.size;
		qoi_rgba_t px{0,0,0,255};
		qoi_rgba_t index[64] = { 0 };

		int run = 0;
		for (int px_pos = 0; px_pos < px_len && bs.remaining() > 0; px_pos += header.channels)
		{
			if (run > 0) {
				run--;
			}
			else {
				int b1 = bs.readOctet();

				if ((b1 & QOI_MASK_2) == QOI_INDEX) {
					px = index[b1 ^ QOI_INDEX];
				}
				else if ((b1 & QOI_MASK_3) == QOI_RUN_8) {
					run = (b1 & 0x1f);
				}
				else if ((b1 & QOI_MASK_3) == QOI_RUN_16) {
					int b2 = bs.readOctet();
					run = (((b1 & 0x1f) << 8) | (b2)) + 32;
				}
				else if ((b1 & QOI_MASK_2) == QOI_DIFF_8) {
					px.rgba.r += ((b1 >> 4) & 0x03) - 1;
					px.rgba.g += ((b1 >> 2) & 0x03) - 1;
					px.rgba.b += (b1 & 0x03) - 1;
				}
				else if ((b1 & QOI_MASK_3) == QOI_DIFF_16) {
					int b2 = bs.readOctet();
					px.rgba.r += (b1 & 0x1f) - 15;
					px.rgba.g += (b2 >> 4) - 7;
					px.rgba.b += (b2 & 0x0f) - 7;
				}
				else if ((b1 & QOI_MASK_4) == QOI_DIFF_24) {
					int b2 = bs.readOctet();
					int b3 = bs.readOctet();
					px.rgba.r += (((b1 & 0x0f) << 1) | (b2 >> 7)) - 15;
					px.rgba.g += ((b2 & 0x7c) >> 2) - 15;
					px.rgba.b += (((b2 & 0x03) << 3) | ((b3 & 0xe0) >> 5)) - 15;
					px.rgba.a += (b3 & 0x1f) - 15;
				}
				else if ((b1 & QOI_MASK_4) == QOI_COLOR) {
					if (b1 & 8) { px.rgba.r = bs.readOctet(); }
					if (b1 & 4) { px.rgba.g = bs.readOctet(); }
					if (b1 & 2) { px.rgba.b = bs.readOctet(); }
					if (b1 & 1) { px.rgba.a = bs.readOctet(); }
				}

				index[QOI_COLOR_HASH(px) % 64] = px;
			}

			if (header.channels == 4) {
				*(qoi_rgba_t*)(pixels + px_pos) = px;
			}
			else {
				pixels[px_pos] = px.rgba.r;
				pixels[px_pos + 1] = px.rgba.g;
				pixels[px_pos + 2] = px.rgba.b;
			}
		}

		return true;
	}


};