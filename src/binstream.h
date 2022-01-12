#pragma once

/*
    This file is a general memory stream interface.

    If you really want to use the streaming features of
    the C++ language, you should not use this file.  
    
    If, on the other hand, you want some simple straight
    forward streaming capabilities, then this file is
    for you. 
*/

#include "apidefs.h"

#include <stdint.h>
#include <string.h>

// Define this here instead of using MIN to
// reduce dependencies
#define BINMIN(a,b) (((a)<(b))?(a):(b))

// These unions help with writing float and double values
typedef union {
    uint32_t u32;
    float f;
} U32float;

typedef union {
    uint64_t u64;
    double d;
} U64double;

// Basic interface for reading 8-bit values from a source
class IReadBinary
{
    virtual uint8_t readOctet() noexcept = 0;
};

// Basic interface for writing 8-bit values to a destination
class IWriteBinary
{
    virtual bool writeOctet(const uint8_t octet) noexcept = 0;
};

// 
// BinStream
// reads and writes to a chunk of memory
// This object will not expand the memory
// 
class BinStream : public IReadBinary, public IWriteBinary
{
    uint8_t *fdata;
    size_t fsize;
    size_t fcursor;
    bool fbigend;

protected:
    BinStream() 
        : fdata(nullptr),
        fbigend(false),
        fcursor(0),
        fsize(0)
    {}

public:

    BinStream(void *data, const size_t size, size_t position=0, bool littleendian = true)
        : fbigend(!littleendian),
        fcursor(position),
        fdata((uint8_t *)data),
        fsize(size)
    {}
 
    inline constexpr bool isValid() const noexcept {return fdata != nullptr;}

    // report whether we've reached the end of the stream yet
    inline bool isEOF() const noexcept {return (remaining() < 1);}
    inline void setBigEndian(bool isBig) noexcept {fbigend = isBig;}
    
    inline void setData(uint8_t* data, size_t size) noexcept
    {
        fdata = data;
        fsize = size;
    }

    inline constexpr uint8_t * data() noexcept {return fdata;}
    inline constexpr size_t size() const noexcept {return fsize;}
    
    // report how many bytes remain to be read
    // from stream
    inline constexpr size_t remaining() const noexcept {return fsize - fcursor;}

    // Report the current cursor position.
    inline constexpr size_t tell() noexcept {return fcursor;}



    // get a subrange of the memory stream
    // returning a new memory stream
    // This is very useful for getting a subset of a data stream
    // with bounds checking.  You must be mindful of the lifetime
    // of the data pointed to though.  There is no provision for 
    // ensuring the data remains valid outside the context of the 
    // original stream.  but, since the data pointer was passed in 
    // from the beginning, that guarantee is already absent.
    BinStream range(size_t pos, size_t size) noexcept
    {
        if (pos < 0 || (fsize < 0)) {
            return BinStream();
        }

        // desired position is beyond end of data
        if (pos >= fsize) {
            return BinStream();
        }

        // The size asked for is greater than the amount remaining
        if ((size > (fsize - pos))) { 
            return BinStream();
        }

        return BinStream(fdata+pos, size, 0 , !fbigend);
    }

    // A convenience to return a range from our current position
    BinStream range(size_t size) noexcept {return range(fcursor, size); }


    // move the position cursor to an absolute offset
    // from the beginning of the data
    // if desired position is out of range, do not
    // reposition, and return false
    bool seek(const size_t pos) noexcept
    {
        // if position specified outside of range
        // just set it past end of stream
        if (pos >= fsize) {
            fcursor = fsize;
            return false;
        }
        
        fcursor = pos;
 
        return true;
    }



    /*
        Moving the cursor around
    */
    // move the cursor ahead by the amount
    // specified in the offset
    // seek, relative to current position
    bool skip(int64_t offset) noexcept {return seek(fcursor + offset);}

    // Seek forwad to a boundary of the specified
    // number of bytes.
    bool alignTo(size_t num) noexcept {return skip(fcursor % num);}
    
    // Skip to the next even numbered offset
    bool skipToEven() noexcept {return alignTo(2);}

    // Return a pointer to the current position
    constexpr void * getPositionPointer() noexcept {return fdata + fcursor;}


    // operators to treat stream as array
    // accessing beyond bounds is undefined, but should
    // not crash
    uint8_t operator[](const size_t offset) const noexcept
    {
        return offset < fsize ? fdata[offset] : 0;
    }

    // setting a value
    // BUGBUG - does not do bounds checking
    uint8_t & operator[](const size_t offset) noexcept
    {
        return fdata[offset];
    }

    // get next 8-bit byte, but don't advance the cursor
    int peekOctet(int offset=0) noexcept
    {
        if (fcursor+offset >= fsize) {
            return -1;
        }

        return fdata[fcursor+offset];
    }


    // get 8 bits, and advance the cursor
    uint8_t readOctet() noexcept
    {
        //print("self.cursor: ", self.cursor, self.size)
        if (fcursor >= fsize) {
            // BUGBUG - throw exception
            return 0;
        }

        fcursor = fcursor + 1;

        //printf("readOctet() - fcursor: %d\n", fcursor);
        
        return fdata[fcursor-1];
    }

    // readBytes
    // Copy bytes from the current position into the supplied
    // buffer.  The number of bytes read is the smaller of
    // the number desired 'n' and the number actually remaining
    // in the data buffer.
    size_t readBytes(uint8_t * buff, const size_t n) noexcept
    {
        // asking for fewer than one bytes
        // returns 0
        if (n < 1) { 
            return 0;
        }

        // see how many bytes are remaining to be read
        size_t nActual = BINMIN(n, remaining());
    
        // read the minimum between remaining and 'n'
        // use memcpy_s as that's probably going to be optimized
        uint8_t * ptr = fdata+fcursor;
        memcpy_s(buff, n, ptr,nActual);

        // move the position cursor ahead by the number of 
        // bytes we actually read.
        skip(nActual);

        // return the number of bytes actually read
        return nActual;
    }

    // readStringZ()
    // read in a null terminated string
    // n - tells us the size of the buffer we're reading into
    // 
    size_t readStringZ(const size_t n, char *buff) noexcept
    {
        // determine maximum number of bytes we can read
        // we leave one space for the null terminator
        size_t nActual = BINMIN(n-1, remaining());
        size_t idx = 0;
        while (idx < nActual) {
            buff[idx] = readOctet();
            if (buff[idx] == 0) {
                break;
            }

            idx = idx + 1;
        }

        // ensure null termination
        buff[idx] = 0;

        return idx;
    }



// Read a single line up to a maximum specified buffer size
// A line is designated with one of the following sequences
// CR-LF
// LF
//
// In either case, the line ending will not be included in the buffer
//
// The buffer is null terminated
size_t readLine(char* buff, const size_t bufflen) noexcept
{
    static const int CR = '\r';
    static const int LF = '\n';

    if (isEOF()) {
        return 0;
    }

    int64_t starting = tell();
    int64_t ending = starting;
    void *startPtr = getPositionPointer();

    while (!isEOF()) {
        uint8_t c = peekOctet();
        if (c < 0) {
            break;
        }

        if (c == CR) {
            c = peekOctet(1);
            if (c == LF) {
                ending = tell();
                skip(2);
                break;
            }
        } else if (c == LF) {
            skip(1);
            break;
        }

        skip(1);
        ending = tell();
    }

    size_t len = ending - starting;
    len = BINMIN(len, bufflen-1);

    memcpy(buff, startPtr, len);
    buff[len] = 0;

    return len;
}

    // readInt()
    // Read an integer value
    // The parameter 'n' determines how many bytes to read.
    // 'n' can be up to 8 
    // The routine will deal with big or little endian
    uint64_t readInt(int n) noexcept
    {
        uint64_t v = 0;
        int i = 0;

        if (remaining() < n) {
            // BUGBUG - throw exception
            return 0;
        }

        if (fbigend) {
            while  (i < n) {
                v = ((v<< 8) | readOctet());
                i = i + 1;
            }
        } else {
            while  (i < n) {
                v = (v | (readOctet() << (8*i)));
                i = i + 1;
            }
        }

        return v;
    }

    // Read 8-bit signed integer
    int8_t readInt8() noexcept {return (int8_t)readInt(1);}

    // Read 8-bit unsigned integer
    uint8_t readUInt8() noexcept {return (uint8_t)readInt(1);}

    // Read 16-bit signed integer
    int16_t readInt16() noexcept {return (int16_t)readInt(2);}

    // Read 16-bit unsigned integer
    uint16_t readUInt16() noexcept {return (uint16_t)readInt(2);}

    // Read Signed 32-bit integer
    int32_t readInt32() noexcept {return (int32_t)readInt(4);}

    // Read unsigned 32-bit integer
    uint32_t readUInt32() noexcept {return (uint32_t)readInt(4);}

    // Read signed 64-bit integer
    int64_t readInt64() noexcept {return (int64_t)readInt(8);}

    uint64_t readUInt64() noexcept {return (uint64_t)readInt(8);}

    float readFloat() noexcept
    {
        U32float f1;
        f1.u32 = readUInt32();
        return f1.f;
    }

    double readDouble() noexcept
    {
        U64double d1;
        d1.u64 = readUInt64();
        return d1.d;
    }

    /*
        Writing to a binary stream
    */
    // Write a single octet to the stream
    inline bool writeOctet(const uint8_t octet) noexcept
    {
        fdata[fcursor] = octet;
        fcursor = fcursor + 1;

        return true;
    }

    // Write the specified number of bytes
    bool writeBytes(const void *bytes, const size_t n) noexcept
    {
        if (bytes == nullptr) {
            return false;
        }

        if (n > remaining()) {
            // BUGBUG - throw exception
            return false;   //, "Not enough space"
        }

        // Do a fast memory copy
        memcpy_s(fdata+fcursor, n, (uint8_t*)bytes, n);
        skip(n);

        return true;
    }

    // Write a null terminated string
    bool writeStringZ(const char * str) noexcept
    {
        if (str == nullptr) {
            return 0;
        }

        size_t n = strlen(str);
        bool success = writeBytes((const uint8_t *)str, n);
        
        // write a null terminating byte
        writeOctet(0);

        return success;
    }

    int writeInt(const uint64_t value, const size_t n) noexcept
    {
        if (remaining() < n) {
            // BUGBUG - throw exception
            return 0;
        }

        int i = n-1;
        if (fbigend) {
            while  (i >= 0) {
                uint8_t v = ((value >> (i * 8)) & 0xff);
                writeOctet(v);
                i = i - 1;
            }
        } else {
            int cnt = 0;
            while  (cnt < n) {
                writeOctet((value >> (cnt*8)) & 0xff);
                cnt = cnt + 1;
            }
        }

        return n;
    }

    size_t writeInt8(const int8_t n) noexcept {return writeInt(n, 1);}
    size_t writeUInt8(const uint8_t n) noexcept {return writeInt(n, 1);}

    size_t writeInt16(int16_t n) noexcept {return writeInt(n, 2);}
    size_t writeUInt16(uint16_t n) noexcept {return writeInt(n, 2);}

    size_t writeInt32(int32_t n) noexcept {return writeInt(n, 4);}
    size_t writeUInt32(uint32_t n) noexcept {return writeInt(n, 4);}

    size_t writeInt64(int64_t n) noexcept {return writeInt(n, 8);}
    size_t writeUInt64(uint64_t n) noexcept {return writeInt(n, 8);}

    void writeFloat(const float value) noexcept
    {
        U32float f1;
        f1.f = value;
        writeUInt32(f1.u32);
    }
    
    void writeFloat(const double value) noexcept
    {
        U64double d1;
        d1.d = value;
        writeUInt64(d1.u64);
    }

    // various useful fixed formats
    // fixed 2_14 numbers
    // 16-bit
    float readF2Dot14() noexcept
    {
        return (float)readUInt16() / 0x4000;
    }

    // fixed 16_16 numbers
    // 32-bit
    float readFixed() noexcept
    {
        uint16_t decimal = readInt16();
        uint16_t fraction = readUInt16();

        return (float)decimal + ((float)fraction / 0xffff);
    }

    // fixed 2_30 numbers
    // 32-bit
    float readF2Dot30() noexcept
    {
        return (float)readUInt32() / 0x4000000;
    }

    // Convenient names used in various documentation
    uint8_t readBYTE()  noexcept {return readOctet();}
    uint16_t readWORD()  noexcept {return readUInt16();}
    uint32_t readDWORD()  noexcept {return  readUInt32();}
    int32_t readLONG()  noexcept {return readInt32();}





/*
readFWord = readInt16;
readUFWord = readUInt16;
readOffset16 = readUInt16;
readOffset32 = readUInt32;
*/
};