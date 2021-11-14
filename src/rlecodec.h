#pragma once

#include "pixelmap.h"
#include "binstream.h"

class LuminanceRLE
{
    struct PacketType
    {
        static const int Raw = 0;
        static const int Rle = 0x80;
    };


    /// <summary>
    /// Determine the packet type that appears in the image.  The type is either
    /// Raw, or Rle.  Raw means that the next color is different than the current one.
    /// Rle means the next color is the same as the current one.
    /// </summary>
    /// <param name="pixMap"></param>
    /// <param name="row"></param>
    /// <param name="pos"></param>
    /// <param name="width"></param>
    /// <returns></returns>
    int RLEPacketType(PixelMap & pixMap, int row, int pos, int width)
    {
        if (pos == width - 1)
            return PacketType::Raw; // single pixel, so return 'Raw' immediately

        byte pixel = grayMaker.toLuminance(pixMap.getPixel(pos, row));
        byte nextPixel = grayMaker.toLuminance(pixMap.getPixel(pos + 1, row));
        bool same = (pixel == nextPixel);

        if (same) // duplicate pixel, so Run Length Encode
        {
            return PacketType::Rle;
        }

        return PacketType::Raw;
    }

    /// <summary>
    /// Find the length of the current packet.  It is either a Rle run, where
    /// we're repeating some color, or it's a Raw run, where every pixel is different.
    /// </summary>
    /// <param name="pixMap"></param>
    /// <param name="row"></param>
    /// <param name="pos"></param>
    /// <param name="width"></param>
    /// <param name="type"></param>
    /// <returns></returns>
    int RLEPacketLength(PixelMap & pixMap, int row, int pos, int width, int type)
    {
        int len = 2;

        // We're at the last pixel in the row,
        // so return '1' immediately.
        if (pos == width - 1)
            return 1;

        // Same for the second to last pixel in the row
        // if it's raw, or Rle, this will be correct
        if (pos == width - 2)
            return 2;

        if (PacketType::Rle == type)
        {
            while (pos + len < width)
            {
                byte pixel = grayMaker.toLuminance(pixMap.getPixel(pos, row));
                byte nextPixel = grayMaker.toLuminance(pixMap.getPixel(pos + len, row));
                bool same = (pixel == nextPixel);

                if (same)
                    len++;
                else
                    return len;

                if (len == 128)
                    return 128;
            }
        }
        else // type == Raw
        {
            while (pos + len < width)
            {
                if (RLEPacketType(pixMap, row, pos + len, width) == PacketType::Raw)
                    len++;
                else
                    return len;

                if (len == 128)
                    return 128;
            }
        }

        return len; // hit end of row (width)
    }

    /// <summary>
    /// Write one row of the image into the stream using the Targa Rle compression technique.
    /// </summary>
    /// <param name="stream"></param>
    /// <param name="src"></param>
    /// <param name="row"></param>
    void WriteRLERow(BinStream &stream, PixelMap & src, int row)
    {
        int pos = 0;

        while (pos < src.width())
        {
            int type = RLEPacketType(src, row, pos, src.width());
            int len = RLEPacketLength(src, row, pos, src.width(), type);
            byte packet_header;

            packet_header = (byte)(len - 1);
            if (PacketType::Rle == type)
                packet_header |= 0x80;

            stream.writeOctet(packet_header);

            if (PacketType::Rle == type)
            {
                // Write a single pixel
                byte pixel = grayMaker.toLuminance(src.getPixel(pos, row));
                stream.writeOctet(pixel);
            }
            else // type is Raw
            {
                // Write a series of pixels starting at the
                // 'pos' and running for 'len' bytes.
                for (int column = 0; column < len; column++)
                {
                    //byte pixel = src.getPixel(pos + column, row).lum();
                    byte pixel = grayMaker.toLuminance(src.getPixel(pos + column, row));
                    stream.writeOctet(pixel);
                }
            }

            pos += len;
        }

    }

public:
    // Encode the pixel map into the stream
    // we only push the raw pixel data, none
    // of the meta data
    void Encode(PixelMap & pixMap, BinStream &stream)
    {
        for (int y = 0; y < pixMap.height(); y++)
        {
            WriteRLERow(stream, pixMap, y);
        }
    }


    // To decode, we need to already know the size
    // of the picture we're decoding
    //
    void Decode(BinStream stream, PixelMap & pixMap)
    {
        // For each row, we want to walk the pixels incrementing 
        // a count of whichever pixels are the same in a run, up 
        // to 128.
        int runLength = 0;
        byte nextPixel=0;
        bool isCompressed = false;
        bool skip;
        byte headerbyte = 0;

        for (int y = 0; y < pixMap.height(); y++)
        {
            for (int x = 0; x < pixMap.width(); x++)
            {
                if (runLength > 0)
                {
                    runLength--;
                    skip = isCompressed;
                }
                else
                {
                    headerbyte = (byte)stream.readOctet();
                    // If the high order bit of the first byte is a '1', then
                    // the next byte represents a compressed run, otherwise it is a raw run
                    isCompressed = ((headerbyte & 0x80) == 0x80);
                    runLength = headerbyte & 0x7f;
                    skip = false;
                }

                if (!skip)
                {
                    // Get the actual data from the stream
                    nextPixel = stream.readOctet();
                }

                pixMap.setPixel(x, y, PixelRGBA( nextPixel,nextPixel,nextPixel ));
            }
        }
    }

private:
    NTSCGray grayMaker;
};
