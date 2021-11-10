/*
	This is here to capture all those things that need to be code
*/
#include "raytracer.h"

// Miscellaneous stuff
/*
const interval interval::empty = interval(+infinity, -infinity);
const interval interval::universe = interval(-infinity, +infinity);

interval operator+(const interval& ival, double displacement) {
    return interval(ival.min + displacement, ival.max + displacement);
}

interval operator+(double displacement, const interval& ival) {
    return ival + displacement;
}
*/



// This is the thread start routine
// It will call the render() function of the 
// scene, passing parameters
static DWORD WINAPI renderScene(void* param)
{
    RayTracer::RTraceCtx* ctx = (RayTracer::RTraceCtx*)param;
    ctx->fScene->renderSegment(ctx->fPixelMap, ctx->fLow, ctx->fHigh);

    // when done, need to delete the context
    // delete ctx

    // The return value is not used
    // but we'll return 1 anyway
    return 1;
}

static DWORD WINAPI renderSceneRow(void* param)
{
    RayTracer::RTraceCtx* ctx = (RayTracer::RTraceCtx*)param;

    while (true) {
        // if the queue is empty, then finish thread
        if (ctx->fRows.size() < 1)
            break;


        int arow = ctx->fRows.front();
        ctx->fRows.pop();
        //std::cerr << "Scanlines remaining: " << arow << ' ' << std::endl << std::flush;

        ctx->fScene->renderSegment(ctx->fPixelMap, arow, arow);
    }

    // The return value is not used
    // but we'll return 1 anyway
    return 1;
}

// This is the primary function called to render the scene
 // Just pass in a pixelmap and number of segments
 // This will use multiple threads
void RayTracer::render(PixelMap* pmap, scene* aScene, const int numSegments)
{
    // Setup the individual segment threads
    int segLength = aScene->height() / numSegments;

    for (int i = 0; i < numSegments; i++)
    {
        int starting = i * segLength;
        int ending = starting + segLength - 1;
        RTraceCtx* ctx = new RTraceCtx(pmap, aScene, ending, starting);
        Thread* t1 = new Thread(renderScene, ctx);
        t1->resume();
    }
}

void RayTracer::renderRows(PixelMap* pmap, scene* aScene, const int numThreads)
{
    RayTracer::RTraceCtx* ctx = new RayTracer::RTraceCtx(pmap, aScene, 0, 0);
    for (int i = 0; i < aScene->height(); i++)
        ctx->fRows.push(i);

    for (int i = 0; i < numThreads; i++)
    {
        Thread* t1 = new Thread(renderSceneRow, ctx);
        t1->resume();
    }
}
