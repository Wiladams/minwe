#pragma once

#include <queue>

#include "rtweekend.h"

#include "scene.h"
#include "thread.h"

//
// The RayTracer is responsible for actually running the 
// ray tracing in multiple threads

// Right now it assigns a number of rows to a single thread
// a better strategy will be to have a thread pool, and allow
// threads to pull a row number from a queue.  When there
// are no more rows in the queue, the thread can quit.
// Once it finishes a row, it can pull another row off the queue
struct RayTracer {
    struct RTraceCtx {
        PixelMap* fPixelMap;
        scene* fScene;

        std::queue<int> fRows;  // queue of rows to be rendered
        int fLow;
        int fHigh;

        RTraceCtx(PixelMap* pmap, scene* s, int starting, int ending)
            : fPixelMap(pmap),
            fScene(s),
            fLow(starting),
            fHigh(ending)
        {
        }
    };


    // This is the primary function called to render the scene
    // Just pass in a pixelmap and number of segments
    // This will use multiple threads
    static void render(PixelMap* pmap, scene* aScene, int numSegments = 1);

    static void renderRows(PixelMap* pmap, scene* aScene, int numThreads);

};