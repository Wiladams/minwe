#include "gui.h"

#include "VfwCameraDevice.h"

VfwCameraDevice camera(0, 640, 480, 30, WS_VISIBLE, nullptr);
VfwDeviceDriver driver(0);

void onFrame()
{
	camera.grabSingleFrame();
}

/*
    When we grab a frame, we then display it
*/
LRESULT __stdcall onFrameGrabbed(HWND hWnd, LPVIDEOHDR lpVHdr)
{
    //printf("==== onFrameGrabbed ====\n");
    //printVideoHeader(lpVHdr);
    //PixelBufferRGB24 pb(640, 480, lpVHdr->lpData);

    int srcX = 0;
    int srcY = 0;
    int srcWidth = 640;
    int srcHeight = 480;
    int x = 0;
    int y = 0;


    // Can't do it this way as it needs to be flipped vertically
    //gAppSurface->blit(pb, srcX, srcY, srcWidth, srcHeight, x, y, srcWidth, srcHeight);

    return 1;
}

void setupCamera()
{
    printf("Driver: [%d - %d] %s\n", driver.getIndex(), driver.isValid(), driver.getName());

    if (!camera.connectToDriver(driver)) {
        printf("failed to connect to driver\n");
        return;
    }

    camera.selectVideoSource();
    camera.selectVideoFormat();


    //camera.setPreviewRate(33);
    //camera.setPreviewScale(1);
    camera.setCallbackOnFrame(onFrameGrabbed);
    camera.hidePreview();
    //camera.startPreview();
}

void setup()
{
    setCanvasSize(640, 480);
    setupCamera();
}