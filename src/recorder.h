#pragma once

//Record a PixelArray
//
// Then, to generate video
//
// ffmpeg -framerate 10 -i fileprefix%06d.ppm moviename.mp4
//

#include "imagecodec.h"
#include "pixeltypes.h"

#include <string>

class Recorder
{
    PPMBinary fCodec;
    PixelArray * fCanvas;

    std::string fBasename;

    bool fIsRecording;
    int fCurrentFrame;
    int fMaxFrames;

public:
    Recorder() = default;

    Recorder(PixelArray* surf, const char* basename = "frame", int maxFrames = 0)
        : fCanvas(surf)
        , fBasename(basename)
        , fIsRecording(false)
        , fMaxFrames(maxFrames)
        , fCurrentFrame(0)
    {
        init(surf, basename, maxFrames);
    }

    void init(PixelArray* surf, const char* basename = "frame", int maxFrames = 0)
    {
        fCanvas = surf;
        fBasename = basename;
        fMaxFrames = maxFrames;
        fCurrentFrame = 0;
    }

    bool saveFrame()
    {
        if (!fIsRecording)
            return false;

        if (fMaxFrames > 0) {
            if (fCurrentFrame >= fMaxFrames) {
                return false;   // reached maximum frames, don't record
            }
        }

        // Create filename based on frame number
        char frameName[256];
        sprintf_s(frameName, 255, "%s%06d.ppm", fBasename.c_str(), fCurrentFrame);

        FILE *fp;
        auto err = fopen_s(&fp, frameName, "wb");

        if (!fp)
            return false;

        fCodec.write(fp, *fCanvas);

        fclose(fp);

        fCurrentFrame = fCurrentFrame + 1;

        return true;
    }

    void toggleRecording()
    {
        if (fIsRecording)
            pause();
        else
            record();
    }

    bool record()
    {
        fIsRecording = true;

        return true;
    }

    void pause()
    {
        fIsRecording = false;
    }

    void stop()
    {
        fCurrentFrame = 0;
        fIsRecording = false;
    }

};

