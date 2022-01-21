
/*
    The primary function of this file is to act as a bridge between 
    the Windows environment, and our desired, fairly platform independent
    application environment.

    All you need to setup a Windows application is this file.  It will 
    operate either in console, or Windows mode.

    This file deals with user input (mouse, keyboard, pointer, joystick, touch)
    initiating a pub/sub system for applications to subscribe to.

    http://www.itimdp4.com/winbugs.html
*/

#include "apphost.h"

#include <shellapi.h>   // for drag-drop support

#include <hidsdi.h>     // USB HID, raw usage definitions
#pragma comment (lib, "hid.lib")

#include <cstdio>
#include <array>
#include <iostream>
#include <memory>



#include "LayeredWindow.hpp"
#include "joystick.h"
#include "network.h"

// Some function signatures
// WinMSGObserver - Function signature for Win32 message handler
typedef LRESULT (*WinMSGObserver)(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);


// Application routines
// appmain looks for this routine in the compiled application
static VOIDROUTINE gOnloadHandler = nullptr;
static VOIDROUTINE gOnUnloadHandler = nullptr;

static VOIDROUTINE gOnLoopHandler = nullptr;

// Painting
static WinMSGObserver gPaintHandler = nullptr;

// Topics applications can subscribe to
SignalEventTopic gSignalEventTopic;
KeyboardEventTopic gKeyboardEventTopic;
MouseEventTopic gMouseEventTopic;
JoystickEventTopic gJoystickEventTopic;
FileDropEventTopic gFileDropEventTopic;
TouchEventTopic gTouchEventTopic;
PointerEventTopic gPointerEventTopic;

// Miscellaneous globals
int gargc;
char **gargv;


User32Window * gAppWindow = nullptr;
std::shared_ptr<User32PixelMap>  gAppSurface = nullptr;

bool gIsLayered = false;

int loopCount = 0;

// Some globals friendly to the p5 environment
// Display Globals
int canvasWidth = 0;
int canvasHeight = 0;

int displayWidth = 0;
int displayHeight= 0;
unsigned int systemDpi = 96;    // 96 == px measurement
unsigned int systemPpi = 192;   // starting pixel density


// Raw Mouse input
int rawMouseX = 0;
int rawMouseY = 0;

// Joystick
static Joystick gJoystick1(JOYSTICKID1);
static Joystick gJoystick2(JOYSTICKID2);




// Controlling drawing
// This function is called at any time to display whatever is in the app 
// window to the actual screen
void refreshScreen ()
{
    if ((gAppSurface == nullptr)) {
        printf("forceRedraw, NULL PTRs\n");
        return;
    }

    if (!gIsLayered) {

        // if we're not layered, then we'll use RedrawWindow
        // to get a WM_PAINT called synchronously.
        // We want to do it this way rather than relying on 
        // WM_ERASEBKGND because then regular WM_PAINT messages
        // can also be responded to, without extra effort
        // We do NOT use InvalidateRect(), because then we have
        // no idea when the system gets around to actually issuing
        // a WM_PAINT
        // This does cause concern for threading though, so that must
        // be thought about.  The painting should occur from the same
        // thread that created the window in the first place
        //InvalidateRect(gAppWindow->getHandle(), NULL, 1);
        //printf("refreshScreen\n");
        //BOOL res = RedrawWindow(gAppWindow->getHandle(), nullptr, nullptr, RDW_ERASE| RDW_INVALIDATE| RDW_ERASENOW);
        
        // This one uses the WM_PAINT message
        BOOL res = RedrawWindow(gAppWindow->getHandle(), nullptr, nullptr, RDW_ERASE | RDW_INVALIDATE | RDW_UPDATENOW);
    }
    else {
        LayeredWindowInfo lw(canvasWidth, canvasHeight);
        lw.display(gAppWindow->getHandle(), gAppSurface->getDC());
    }
}


/*
    Environment
*/
void show()
{
    gAppWindow->show();
}

void hide()
{
    gAppWindow->hide();
}

// Show the cursor, if there is one
void cursor()
{
    int count = ::ShowCursor(1);
}


// BUGBUG - we should be more robust here and
// check to see if there's even a mouse attached
// Then, decrement count enough times to make showing
// the cursor absolute rather than relative.
void noCursor()
{
    ::ShowCursor(0);
}

/*
    Subscription routines
*/
// General signal subscription
void subscribe(SignalEventTopic::Subscriber s)
{
    gSignalEventTopic.subscribe(s);
}

// Allow subscription to keyboard events
void subscribe(KeyboardEventTopic::Subscriber s)
{
    gKeyboardEventTopic.subscribe(s);
}

// Allow subscription to mouse events
void subscribe(MouseEventTopic::Subscriber s)
{
    gMouseEventTopic.subscribe(s);
}

// Allow subscription to mouse events
void subscribe(JoystickEventTopic::Subscriber s)
{
    gJoystickEventTopic.subscribe(s);
}

void subscribe(FileDropEventTopic::Subscriber s)
{
    gFileDropEventTopic.subscribe(s);
}

void subscribe(TouchEventTopic::Subscriber s)
{
    gTouchEventTopic.subscribe(s);
}

void subscribe(PointerEventTopic::Subscriber s)
{
    gPointerEventTopic.subscribe(s);
}


//
// Handling messages
//

//
//  https://docs.microsoft.com/en-us/windows/desktop/inputdev/using-raw-input
//  https://www.codeproject.com/articles/185522/using-the-raw-input-api-to-process-joystick-input
//  https://github.com/supersmo/Using-Raw-Input-API-to-Process-Joystick-Input/blob/master/RawInput.c


// Register a device for raw input
bool HID_RegisterDevice(HWND hTarget, USHORT usage, USHORT usagePage = HID_USAGE_PAGE_GENERIC, USHORT flags = 0)
{
    RAWINPUTDEVICE hid;

    hid.usUsagePage = usagePage;
    hid.usUsage = usage;
    hid.dwFlags = flags; // (RIDEV_DEVNOTIFY | RIDEV_INPUTSINK | RIDEV_NOLEGACY);
    hid.hwndTarget = hTarget;


    BOOL bResult = ::RegisterRawInputDevices(&hid, 1, sizeof(RAWINPUTDEVICE));
    if (bResult == 0)
    {
        auto res = ::GetLastError();
        printf("HID_RegisterDevice Error: 0x%x\n", res);
        return false;
    }

    // successful registration
    return true;
}

void HID_UnregisterDevice(USHORT usage)
{
    RAWINPUTDEVICE hid{ 0 };
    hid.usUsagePage = 1;
    hid.usUsage = usage;
    hid.dwFlags = RIDEV_REMOVE;
    hid.hwndTarget = nullptr;
    UINT uiNumDevices = 1;

    BOOL bResult = ::RegisterRawInputDevices(&hid, 1, sizeof(RAWINPUTDEVICE));
}


void ParseRawInput(PRAWINPUT raw)
{
    // See what we got
    //printf("RAWINPUT: %d\n", raw->header.dwType);

    // For mouse and keyboard, there are data structures that already
    // parse the contents
    // Got others, the data needs to be parsed
    switch (raw->header.dwType)
    {
        case RIM_TYPEMOUSE:
        {
            rawMouseX = raw->data.mouse.lLastX;
            rawMouseY = raw->data.mouse.lLastY;
            //mouseEvent();
            //printf("RAWMOUSE: %d %d\n", raw->data.mouse.lLastX, raw->data.mouse.lLastY);
        }
        break;

        case RIM_TYPEKEYBOARD:
        {
            //keyboardEvent
        }
        break;

        case RIM_TYPEHID:
        {
            PHIDP_PREPARSED_DATA pPreparsedData = nullptr;
            UINT bufferSize = 0;

            // get the name of the device
            //char nameBuff[1024] = {0};
            //UINT nameBuffSize = 1024;
            //GetRawInputDeviceInfo(raw->header.hDevice, RIDI_DEVICENAME, nameBuff, &nameBuffSize);

            /*
            // Get the device info, using two phase allocation
            GetRawInputDeviceInfo(raw->header.hDevice, RIDI_PREPARSEDDATA, NULL, &bufferSize);
            std::vector<uint8_t> buff(bufferSize, 0);
            pPreparsedData = (PHIDP_PREPARSED_DATA)buff.data();
            GetRawInputDeviceInfo(raw->header.hDevice, RIDI_PREPARSEDDATA, pPreparsedData, &bufferSize);

            // Get the device capabilities
            // Button caps
            HIDP_CAPS            Caps;
            PHIDP_BUTTON_CAPS    pButtonCaps = nullptr;
            USHORT capsLength = 0;

            auto res = HidP_GetCaps(pPreparsedData, &Caps);
            std::vector<uint8_t> buttonCapsBuff(sizeof(HIDP_BUTTON_CAPS) * Caps.NumberInputButtonCaps, 0);
            pButtonCaps = (PHIDP_BUTTON_CAPS)buttonCapsBuff.data();

            capsLength = Caps.NumberFeatureButtonCaps;
            HidP_GetButtonCaps(HidP_Input, pButtonCaps, &capsLength, pPreparsedData);
            int g_NumberOfButtons = pButtonCaps->Range.UsageMax - pButtonCaps->Range.UsageMin + 1;
            */
            // It's not one of the easy ones, so need
            // to parse
            //printf("HID INPUT: 0x%p\n", raw->header.hDevice);
        }
        break;

        default:
            printf("RAW UNKNOWN TYPE: %d\n", raw->header.dwType);
        break;
    }
}

LRESULT HandleHIDMessage(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    LRESULT res = 0;

    //printf("WM_INPUT\n");
    bool inBackground = GET_RAWINPUT_CODE_WPARAM(wParam) == 1;
    HRAWINPUT inputHandle = (HRAWINPUT)lParam;
    UINT uiCommand = RID_INPUT;
    UINT cbSize = 0;

    // First, find out how much space will be needed
    // to accomodate the HID message
    UINT size = ::GetRawInputData((HRAWINPUT)lParam, RID_INPUT, nullptr, &cbSize, sizeof(RAWINPUTHEADER));


    // Allocate space, and try it again
    std::vector<uint8_t> buff(cbSize, 0);
    size = ::GetRawInputData((HRAWINPUT)lParam, uiCommand, buff.data(), &cbSize, sizeof(RAWINPUTHEADER));
    //printf("WM_INPUT: %d - %d\n", cbSize, size);

    // confirm we got what we expected in terms of size
    if (size == cbSize) {
        RAWINPUT* raw = (RAWINPUT*)buff.data();

        ParseRawInput(raw);

    }

    return res;
}

/*
    Turn Windows keyboard messages into keyevents that can 
    more easily be handled at the application level
*/
LRESULT HandleKeyboardMessage(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    LRESULT res = 0;
    KeyboardEvent e;
    e.keyCode = LOWORD(wParam);
    e.repeatCount =LOWORD(lParam);  // 0 - 15
    e.scanCode = ((lParam & 0xff0000) >> 16);        // 16 - 23
    e.isExtended = (lParam & 0x1000000) != 0;    // 24
    e.wasDown = (lParam & 0x40000000) != 0;    // 30

//printf("HandleKeyboardMessage: %04x\n", msg);

    switch(msg) {
        case WM_KEYDOWN:
        case WM_SYSKEYDOWN:
            e.activity = KEYPRESSED;

            break;
        case WM_KEYUP:
        case WM_SYSKEYUP:
            e.activity = KEYRELEASED;

            break;
        case WM_CHAR:
        case WM_SYSCHAR:
            e.activity = KEYTYPED;
            break;
    }
    
    // publish keyboard event
    gKeyboardEventTopic.notify(e);

    return res;
}


/*
    Turn Windows mouse messages into mouse events which can
    be dispatched by the application.
*/
LRESULT HandleMouseMessage(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{   
    LRESULT res = 0;
    MouseEvent e;

    e.x = GET_X_LPARAM(lParam);
    e.y = GET_Y_LPARAM(lParam);

    auto fwKeys = GET_KEYSTATE_WPARAM(wParam);
    e.control = (fwKeys & MK_CONTROL) != 0;
    e.shift = (fwKeys & MK_SHIFT) != 0;

    e.lbutton = (fwKeys & MK_LBUTTON) != 0;
    e.rbutton = (fwKeys & MK_RBUTTON) != 0;
    e.mbutton = (fwKeys & MK_MBUTTON) != 0;
    e.xbutton1 = (fwKeys & MK_XBUTTON1) != 0;
    e.xbutton2 = (fwKeys & MK_XBUTTON2) != 0;
    bool isPressed = e.lbutton || e.rbutton || e.mbutton;

    //printf("MOUSE: %d\n", msg);
    switch(msg) {
        case WM_LBUTTONDBLCLK:
	    case WM_MBUTTONDBLCLK:
	    case WM_RBUTTONDBLCLK:
            break;

        case WM_MOUSEMOVE:
            e.activity = MOUSEMOVED;
            break;

        case WM_LBUTTONDOWN:
        case WM_RBUTTONDOWN:
        case WM_MBUTTONDOWN:
        case WM_XBUTTONDOWN:
            e.activity = MOUSEPRESSED;
            break;
        case WM_LBUTTONUP:
        case WM_RBUTTONUP:
        case WM_MBUTTONUP:
        case WM_XBUTTONUP:
            e.activity = MOUSERELEASED;
            break;
        case WM_MOUSEWHEEL:
            e.activity = MOUSEWHEEL;
            e.delta = GET_WHEEL_DELTA_WPARAM(wParam);
            break;
        case WM_MOUSEHWHEEL:
            e.activity = MOUSEHWHEEL;
            e.delta = GET_WHEEL_DELTA_WPARAM(wParam);
            break;
            
        break;
    }

    gMouseEventTopic.notify(e);

    return res;
}




// 
// Handling the joystick messages through the Windows
// Messaging method is very limited.  It will only 
// trigger for a limited set of buttons and axes movements
// This handler is here for a complete API.  If the user app
// wants to get more out of the joystick, it can access the
// joystick directly with 'gJoystick1' or 'gJoystick2' and
// call getPosition() at any time.  Typically during 'update()'
// or 'draw()'
LRESULT HandleJoystickMessage(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    LRESULT res = 0;

    JoystickEvent e;

    // We can handle up to two joysticks using
    // this mechanism
    switch (msg) {
    case MM_JOY1BUTTONDOWN:
        gJoystick1.getPosition(e);
        e.activity = JOYPRESSED;
        break;

    case MM_JOY2BUTTONDOWN:
        gJoystick2.getPosition(e);
        e.activity = JOYPRESSED;
        break;

    case MM_JOY1BUTTONUP:
        gJoystick1.getPosition(e);
        e.activity = JOYRELEASED;
        break;
    case MM_JOY2BUTTONUP:
        gJoystick2.getPosition(e);
        e.activity = JOYRELEASED;
        break;

    case MM_JOY1MOVE:
        gJoystick1.getPosition(e);
        e.activity = JOYMOVED;
        break;
    case MM_JOY2MOVE:
        gJoystick2.getPosition(e);
        e.activity = JOYMOVED;
        break;

    case MM_JOY1ZMOVE:
        gJoystick1.getPosition(e);
        e.activity = JOYZMOVED;
    break;
    case MM_JOY2ZMOVE:
        gJoystick2.getPosition(e);
        e.activity = JOYZMOVED;
        break;
    }

    gJoystickEventTopic.notify(e);

    return res;
}

LRESULT HandleTouchMessage(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    LRESULT res = 0;

    // cInputs could be set to a maximum value (10) and
    // we could reuse the same allocated array each time
    // rather than allocating a new one each time.
    int cInputs = LOWORD(wParam);
    int cbSize = sizeof(TOUCHINPUT);

    //printf("HandleTouchMessage 1.0: %d\n", cInputs);
    PTOUCHINPUT pInputs = new TOUCHINPUT[cInputs];

    // 0 == failure?
    BOOL bResult = GetTouchInputInfo((HTOUCHINPUT)lParam, cInputs, pInputs, cbSize);

    if (bResult == 0) {
        delete[] pInputs;
        auto err = ::GetLastError();
        printf("getTouchInputInfo, ERROR: %d %d\n", bResult, err);

        return 0;
    }

    // construct an event for each item
    // BUGBUG - really this should be a deque of events
    // like with file drops
    for (int i = 0; i < cInputs; i++) {
        TOUCHINPUT ti = pInputs[i];
        TouchEvent e = {};

        // convert values to local TouchEvent structure
        e.id = ti.dwID;
        e.rawX = ti.x;
        e.rawY = ti.y;

        POINT PT;
        PT.x = TOUCH_COORD_TO_PIXEL(ti.x);
        PT.y = TOUCH_COORD_TO_PIXEL(ti.y);

        // Convert from screen coordinates to local
        // client coordinates
        auto bResult = ::ScreenToClient(hwnd, &PT);

        // Assign x,y the client area value of the touch point
        e.x = PT.x;
        e.y = PT.y;


        // Now, deal with masks
        //#define TOUCHINPUTMASKF_TIMEFROMSYSTEM  0x0001  // the dwTime field contains a system generated value
        //#define TOUCHINPUTMASKF_EXTRAINFO       0x0002  // the dwExtraInfo field is valid
        //#define TOUCHINPUTMASKF_CONTACTAREA     0x0004  // the cxContact and cyContact fields are valid

        if ((ti.dwMask & TOUCHINPUTMASKF_CONTACTAREA) != 0) {
            e.rawWidth = ti.cxContact;
            e.rawHeight = ti.cyContact;
            e.w = TOUCH_COORD_TO_PIXEL(e.rawWidth);
            e.h = TOUCH_COORD_TO_PIXEL(e.rawHeight);
        }

        // figure out kind of activity and attributes
        /*
        #define TOUCHEVENTF_MOVE            0x0001
#define TOUCHEVENTF_DOWN            0x0002
#define TOUCHEVENTF_UP              0x0004
#define TOUCHEVENTF_INRANGE         0x0008
#define TOUCHEVENTF_PRIMARY         0x0010
#define TOUCHEVENTF_NOCOALESCE      0x0020
#define TOUCHEVENTF_PEN             0x0040
#define TOUCHEVENTF_PALM            0x0080
        */
        if (ti.dwFlags & TOUCHEVENTF_DOWN) {
            e.activity = TOUCH_DOWN;
            e.isDown = true;
        }

        if (pInputs[i].dwFlags & TOUCHEVENTF_UP) {
            e.activity = TOUCH_UP;
            e.x = -1;
            e.y = -1;
            e.isUp = true;
        }

        if (pInputs[i].dwFlags & TOUCHEVENTF_MOVE) {
            e.activity = TOUCH_MOVE;
            e.isMoving = true;
        }

        // Attributes of the event
        if (pInputs[i].dwFlags & TOUCHEVENTF_INRANGE) {
            e.isHovering = true;
        }

        if (ti.dwFlags & TOUCHEVENTF_PRIMARY) {
            e.isPrimary = true;
        }

        if (pInputs[i].dwFlags & TOUCHEVENTF_PALM) {
            e.isPalm = true;
        }
        
        if (pInputs[i].dwFlags & TOUCHEVENTF_PEN) {
            e.isPen = true;
        }

        gTouchEventTopic.notify(e);
    }
    delete[] pInputs;

    if (!CloseTouchInputHandle((HTOUCHINPUT)lParam))
    {
        // error handling
    }

    return res;
}

LRESULT HandlePointerMessage(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    LRESULT res = 0;

    PointerEvent e;

    gPointerEventTopic.notify(e);

    return res;
}

LRESULT HandlePaintMessage(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    //printf("HandlePaintMessage\n");

    LRESULT res = 0;
    PAINTSTRUCT ps;

	HDC hdc = BeginPaint(hWnd, &ps);
        
    int xDest = 0;
    int yDest = 0;
    int DestWidth = canvasWidth;
    int DestHeight = canvasHeight;
    int xSrc = 0;
    int ySrc = 0;
    int SrcWidth = canvasWidth;
    int SrcHeight = canvasHeight;

    BITMAPINFO info = gAppSurface->getBitmapInfo();

    int pResult = StretchDIBits(hdc,
        xDest,yDest,
        DestWidth,DestHeight,
        xSrc,ySrc,
        SrcWidth, SrcHeight,
        gAppSurface->getData(),&info,
        DIB_RGB_COLORS,
        SRCCOPY);
        
	EndPaint(hWnd, &ps);

    return res;
}

LRESULT HandleFileDropMessage(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    LRESULT res = 0;
    HDROP dropHandle = (HDROP)wParam;
    

    // if we have a drop handler, then marshall all the file names
    // and call the event handler. 
    // If the handler does not exist, don't bother with all the work

    FileDropEvent e;
    // Find out where the drop occured
    POINT pt;
    ::DragQueryPoint(dropHandle, &pt);
    e.x = pt.x;
    e.y = pt.y;

    // First, find out how many files were dropped
    auto n = ::DragQueryFileA(dropHandle, 0xffffffff, nullptr, 0);

    // Now that we know how many, query individual files
    // FileDropEvent
    char namebuff[512];

    if (n > 0) {
        for (size_t i = 0; i < n; i++) {
            ::DragQueryFileA(dropHandle, (UINT)i, namebuff, 512);
            e.filenames.push_back(std::string(namebuff));
        }

        gFileDropEventTopic.notify(e);
    }

    ::DragFinish(dropHandle);

    return res;
}








// Controlling the runtime
void halt() 
{
    ::PostQuitMessage(0);
}


void rawMouse()
{
    HWND localWindow = gAppWindow->getHandle();
    HID_RegisterDevice(gAppWindow->getHandle(), HID_USAGE_GENERIC_MOUSE);
}

void rawKeyboard()
{
    HWND localWindow = gAppWindow->getHandle();
    HID_RegisterDevice(gAppWindow->getHandle(), HID_USAGE_GENERIC_KEYBOARD);
}

void rawJoystick()
{
    HID_RegisterDevice(gAppWindow->getHandle(), HID_USAGE_GENERIC_JOYSTICK);
}

void rawInput()
{
    rawMouse();
    rawKeyboard();
    rawJoystick();
}

void noRawInput()
{
    // unregister devices
    HID_UnregisterDevice(HID_USAGE_GENERIC_MOUSE);
    HID_UnregisterDevice(HID_USAGE_GENERIC_KEYBOARD);
    HID_UnregisterDevice(HID_USAGE_GENERIC_JOYSTICK);
}

void legacyJoystick()
{
    //MMRESULT res = joySetCapture(localWindow,JOYSTICKID1, 0,1);
    gJoystick1.attachToWindow(gAppWindow->getHandle());
    gJoystick2.attachToWindow(gAppWindow->getHandle());
}

void noJoystick()
{
    gJoystick1.detachFromWindow();
    gJoystick2.detachFromWindow();
}

// Turning Touch input on and off
bool touch()
{
    BOOL bResult = ::RegisterTouchWindow(gAppWindow->getHandle(), 0);
    return (bResult != 0);
}

bool noTouch()
{
    BOOL bResult = ::UnregisterTouchWindow(gAppWindow->getHandle());
    return (bResult != 0);
}

bool isTouch()
{
    ULONG flags = 0;
    BOOL bResult = ::IsTouchWindow(gAppWindow->getHandle(), &flags);
    return (bResult != 0);
}

// Turning drop file support on and off
bool dropFiles()
{
    ::DragAcceptFiles(gAppWindow->getHandle(),TRUE);
    return true;
}

bool noDropFiles()
{
    ::DragAcceptFiles(gAppWindow->getHandle(), FALSE);
    return true;
}

//
// Window management
//
static LONG gLastWindowStyle=0;

void layered()
{
    gAppWindow->setExtendedStyle(WS_EX_LAYERED|WS_EX_NOREDIRECTIONBITMAP);
    gLastWindowStyle = gAppWindow->setWindowStyle(WS_POPUP);

    gIsLayered = true;
}

void noLayered()
{
    gAppWindow->clearExtendedStyle(WS_EX_LAYERED|WS_EX_NOREDIRECTIONBITMAP);
    gAppWindow->setWindowStyle(gLastWindowStyle);

    gIsLayered = false;
}

bool isLayered()
{
    return gIsLayered;
}

void setWindowPosition(int x, int y)
{
    gAppWindow->moveTo(x, y);
}

bool setCanvasSize(long aWidth, long aHeight)
{
    // Create new drawing surface
    if (gAppSurface != nullptr) {
        // Delete old one if it exists
        //delete gAppSurface;
        //gAppSurface.reset();
    }


    gAppSurface = std::make_shared<User32PixelMap>(aWidth, aHeight);
    canvasWidth = aWidth;
    canvasHeight = aHeight;
    
    if (gAppWindow != nullptr)
        gAppWindow->setCanvasSize(aWidth, aHeight);

    return true;
}


// A basic Windows event loop
void showAppWindow()
{
    gAppWindow->show();
}

/*
    Generic Windows message handler
    This is used as the function to associate with a window class
    when it is registered.
*/
LRESULT CALLBACK MsgHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    LRESULT res = 0;
    //printf("MSG: 0x%x\n", msg);

    if ((msg >= WM_MOUSEFIRST) && (msg <= WM_MOUSELAST)) {
        // Handle all mouse messages
        HandleMouseMessage(hWnd, msg, wParam, lParam);
    }
    else if (msg == WM_INPUT) {
        res = HandleHIDMessage(hWnd, msg, wParam, lParam);
    }
    else if (msg == WM_DESTROY) {
        // By doing a PostQuitMessage(), a 
        // WM_QUIT message will eventually find its way into the
        // message queue.
        ::PostQuitMessage(0);
        return 0;
    }
    else if ((msg >= WM_KEYFIRST) && (msg <= WM_KEYLAST)) {
        // Handle all keyboard messages
        HandleKeyboardMessage(hWnd, msg, wParam, lParam);
    }
    else if ((msg >= MM_JOY1MOVE) && (msg <= MM_JOY2BUTTONUP)) 
    {
        // Legacy joystick messages
        //printf("MM_JOYxxx: %p\n", gJoystickHandler);
        HandleJoystickMessage(hWnd, msg, wParam, lParam);
    }
    else if (msg == WM_TOUCH) {
        // Handle touch specific messages
        //std::cout << "WM_TOUCH" << std::endl;
        HandleTouchMessage(hWnd, msg, wParam, lParam);
    }
    //else if (msg == WM_GESTURE) {
    // we will only receive WM_GESTURE if not receiving WM_TOUCH
    //}
    //else if ((msg >= WM_NCPOINTERUPDATE) && (msg <= WM_POINTERROUTEDRELEASED)) {
    //    HandlePointerMessage(hWnd, msg, wParam, lParam);
    //}
    else if (msg == WM_ERASEBKGND) {
        //loopCount = loopCount + 1;
        //printf("WM_ERASEBKGND: %d\n", loopCount);
        if (gPaintHandler != nullptr) {
            gPaintHandler(hWnd, msg, wParam, lParam);
        }

        // return non-zero indicating we dealt with erasing the background
        res = 1;
    }
    else if (msg == WM_PAINT) {
        if (gPaintHandler != nullptr) 
        {
                gPaintHandler(hWnd, msg, wParam, lParam);
        }
    }
    else if (msg == WM_WINDOWPOSCHANGING) {
        if (gPaintHandler != nullptr) 
        {
            gPaintHandler(hWnd, msg, wParam, lParam);
        }
    }
    else if (msg == WM_DROPFILES) {
        HandleFileDropMessage(hWnd, msg, wParam, lParam);
    }
    else {
        // Not a message we want to handle specifically
        res = ::DefWindowProcA(hWnd, msg, wParam, lParam);
    }

    return res;
}

//
// Look for the dynamic routines that will be used
// to setup client applications.
// Most notable is 'onLoad()' and 'onUnload'
//
void registerHandlers()
{
    // we're going to look within our own module
    // to find handler functions.  This is because the user's application should
    // be compiled with the application, so the exported functions should
    // be attainable using 'GetProcAddress()'

    HMODULE hInst = ::GetModuleHandleA(NULL);

    // Start with our default paint message handler
    gPaintHandler = HandlePaintMessage;


    // One of the primary handlers the user can specify is 'onPaint'.  
    // If implemented, this function will be called whenever a WM_PAINT message
    // is seen by the application.
    WinMSGObserver handler = (WinMSGObserver)::GetProcAddress(hInst, "onPaint");
    if (handler != nullptr) {
        gPaintHandler = handler;
    }

    // Get the general app routines
    // onLoad()
    gOnloadHandler = (VOIDROUTINE)::GetProcAddress(hInst, "onLoad");
    gOnUnloadHandler = (VOIDROUTINE)::GetProcAddress(hInst, "onUnload");

    gOnLoopHandler = (VOIDROUTINE)::GetProcAddress(hInst, "onLoop");
}


void run()
{
    // Make sure we have all the event handlers connected
    registerHandlers();

    // call the application's 'onLoad()' if it exists
    if (gOnloadHandler != nullptr) {
        gOnloadHandler();
    }

    // Do a typical Windows message pump
    MSG msg;
    LRESULT res;

    showAppWindow();

    while (true) {
        // we use peekmessage, so we don't stall on a GetMessage
        // should probably throw a wait here
        // WaitForSingleObject
        BOOL bResult = ::PeekMessageA(&msg, NULL, 0, 0, PM_REMOVE);
        
        if (bResult > 0) {
            // If we see a quit message, it's time to stop the program
            if (msg.message == WM_QUIT) {
                break;
            }

            res = ::TranslateMessage(&msg);
            res = ::DispatchMessageA(&msg);
        }
        //else 
		{
            // call onLoop() if it exists
            if (gOnLoopHandler != nullptr) {
                gOnLoopHandler();
            }
        }
    }
    

}

// Declare some standard Window Kinds we'll be using
User32WindowClass gAppWindowKind("appwindow", CS_GLOBALCLASS | CS_DBLCLKS | CS_HREDRAW | CS_VREDRAW, MsgHandler);

// do any initialization that needs to occur 
// in the very beginning
bool prolog()
{
    // Initialize Windows networking API
    uint16_t version = MAKEWORD(2, 2);
    WSADATA lpWSAData;
    int res = ::WSAStartup(version, &lpWSAData);


    // Throughout the application, we want to know the true
    // physical dots per inch and screen resolution, so the
    // first thing to do is to let Windows know we are Dpi aware
    // set awareness based on monitor, and get change messages when it changes
    //auto dpiContext = DPI_AWARENESS_CONTEXT_SYSTEM_AWARE;
    ////auto dpiContext = DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2;
    //DPI_AWARENESS_CONTEXT oldContext = ::SetThreadDpiAwarenessContext(dpiContext);
    auto bResult = ::SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);

    // based on logical inches
    systemDpi = ::GetDpiForSystem();

    // Get the screen size
    auto dpiDpi = ::GetDpiFromDpiAwarenessContext(DPI_AWARENESS_CONTEXT_SYSTEM_AWARE);
    auto dpidisplayWidth = ::GetSystemMetricsForDpi(SM_CXSCREEN, systemDpi);
    auto dpidisplayHeight = ::GetSystemMetricsForDpi(SM_CYSCREEN, systemDpi);

    displayWidth = ::GetSystemMetrics(SM_CXSCREEN);
    displayHeight = ::GetSystemMetrics(SM_CYSCREEN);

    auto dhdc = ::CreateDC(TEXT("DISPLAY"), NULL, NULL, NULL);

    // Get screen physical size
    // DeviceCaps gives it in millimeters, so we convert to inches
    auto screenWidth = ::GetDeviceCaps(dhdc, HORZSIZE)/25.4;
    auto screenHeight = ::GetDeviceCaps(dhdc, VERTSIZE)/25.4;

    // pixel count horizontally and vertically
    auto pixelWidth = ::GetDeviceCaps(dhdc, LOGPIXELSX);
    auto pixelHeight = ::GetDeviceCaps(dhdc, LOGPIXELSY);

    // Calculate real pixel density
    double screenHPpi = (double)dpidisplayWidth / screenWidth;
    double screenVPpi = (double)dpidisplayHeight / screenHeight;
    systemPpi = (unsigned int)screenVPpi;

    // set the canvas a default size to start
    // but don't show it
    setCanvasSize(640, 480);

    gAppWindow = gAppWindowKind.createWindow("Application Window", 640, 480);

    return true;
}

// Allow cleanup before exit
void epilog()
{
    if (gOnUnloadHandler != nullptr) {
        gOnUnloadHandler();
    }
}


/*
    The 'main()' function is in here to ensure that compiling
    this file will result in an executable program.

    Why on Earth are there two 'main' routines in here?
    the first one 'main()' will be called when the code is compiled
    as a CONSOLE subsystem.

    the second one 'WinMain' will be called when the code is compiled
    as a WINDOWS subsystem.  
    
    By having both, we kill two birds with one stone.
*/
int minweRun()
{
    if (!prolog()) {
        printf("error in prolog\n");
        return -1;
    }

    // Start running main event loop
    run();

    // do any cleanup after all is done
    epilog();

    return 0;
}

int main(int argc, char **argv)
{
    gargc = argc;
    gargv = argv;
    
    return minweRun();
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prev, LPSTR cmdLine, int nShowCmd)
{
    // BUGBUG, need to parse those command line arguments

    return minweRun();
}