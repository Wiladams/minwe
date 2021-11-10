#pragma once

//Record a PixelMap
//
// Then, to generate video
//
// ffmpeg -framerate 10 -i fileprefix%06d.ppm moviename.mp4
//

#include "imagecodec.h"
#include "pixelmap.h"

#include <string>

class Recorder
{
    PPMBinary fCodec;
    PixelMap * fSurface;

    std::string fBasename;

    bool fIsRecording;
    int fCurrentFrame;
    int fMaxFrames;

    Recorder() = delete;    // Don't want default constructor

public:
    Recorder(PixelMap* surf, const char* basename = "frame", int maxFrames = 0)
        : fSurface(surf)
        , fBasename(basename)
        , fIsRecording(false)
        , fMaxFrames(maxFrames)
        , fCurrentFrame(0)
    {
    }


    void saveFrame()
    {
        if (!fIsRecording)
            return;

        if (fMaxFrames > 0) {
            if (fCurrentFrame >= fMaxFrames) {
                return;   // reached maximum frames, maybe stop timer
            }
        }

        char frameName[256];
        sprintf_s(frameName, 255, "%s%06d.ppm", fBasename.c_str(), fCurrentFrame);
        fCodec.write(frameName, *fSurface);

        fCurrentFrame = fCurrentFrame + 1;
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
        if (fIsRecording)
            return false;

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

