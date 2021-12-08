#pragma once

/*
    This should live somewhere else, higher in the stack
*/
struct DataChunk {
private:
    void* fData;
    size_t fSize;
    bool fIOwnData;

public:
    DataChunk(void* data, const size_t size)
        :fData(data),
        fSize(size),
        fIOwnData(false)
    {
    }

    DataChunk(const size_t size)
    {
        fData = { new char[size] {} };
        fSize = size;
        fIOwnData = true;
    }

    ~DataChunk()
    {
        if (fIOwnData) {
            delete fData;
        }
        fSize = 0;
    }

    // Returning properties
    void* data() { return fData; }
    size_t size() { return fSize; }
};
