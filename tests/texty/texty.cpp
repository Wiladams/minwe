#include "gui.h"
#include "draw2d.h"
#include "wingdi.h"

#include <list>

// setting colors
// COLORREF - 0x00bbggrr
//
// PixelRGBA - 0xaarrggbb
//
/*
void textAlign(ALIGNMENT horizontal, ALIGNMENT vertical) noexcept;
void textFont(const char* fontname) noexcept;
void textSize(double size) noexcept;
void text(const char* txt, double x, double y) noexcept;
*/
/*
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    PAINTSTRUCT ps;
    HDC hdc;
    switch (message)
    {


    case WM_PAINT:
    {
        RECT rect;
        HBRUSH hBrush;
        HFONT hFont;
        hdc = BeginPaint(hWnd, &ps);


        //Logical units are device dependent pixels, so this will create a handle to a logical font that is 48 pixels in height.
        //The width, when set to 0, will cause the font mapper to choose the closest matching value.
        //The font face name will be Impact.
        hFont = CreateFont(48, 0, 0, 0, FW_DONTCARE, FALSE, TRUE, FALSE, DEFAULT_CHARSET, OUT_OUTLINE_PRECIS,
            CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, VARIABLE_PITCH, TEXT("Impact"));
        SelectObject(hdc, hFont);

        //Sets the coordinates for the rectangle in which the text is to be formatted.
        SetRect(&rect, 100, 100, 700, 200);
        SetTextColor(hdc, RGB(255, 0, 0));
        DrawText(hdc, TEXT("Drawing Text with Impact"), -1, &rect, DT_NOCLIP);


        //Logical units are device dependent pixels, so this will create a handle to a logical font that is 36 pixels in height.
        //The width, when set to 20, will cause the font mapper to choose a font which, in this case, is stretched.
        //The font face name will be Times New Roman.  This time nEscapement is at -300 tenths of a degree (-30 degrees)
        hFont = CreateFont(36, 20, -300, 0, FW_DONTCARE, FALSE, TRUE, FALSE, DEFAULT_CHARSET, OUT_OUTLINE_PRECIS,
            CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, VARIABLE_PITCH, TEXT("Times New Roman"));
        SelectObject(hdc, hFont);

        //Sets the coordinates for the rectangle in which the text is to be formatted.
        SetRect(&rect, 100, 200, 900, 800);
        SetTextColor(hdc, RGB(0, 128, 0));
        DrawText(hdc, TEXT("Drawing Text with Times New Roman"), -1, &rect, DT_NOCLIP);


        //Logical units are device dependent pixels, so this will create a handle to a logical font that is 36 pixels in height.
        //The width, when set to 10, will cause the font mapper to choose a font which, in this case, is compressed. 
        //The font face name will be Arial. This time nEscapement is at 250 tenths of a degree (25 degrees)
        hFont = CreateFont(36, 10, 250, 0, FW_DONTCARE, FALSE, TRUE, FALSE, DEFAULT_CHARSET, OUT_OUTLINE_PRECIS,
            CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY, VARIABLE_PITCH, TEXT("Arial"));
        SelectObject(hdc, hFont);

        //Sets the coordinates for the rectangle in which the text is to be formatted.
        SetRect(&rect, 500, 200, 1400, 600);
        SetTextColor(hdc, RGB(0, 0, 255));
        DrawText(hdc, TEXT("Drawing Text with Arial"), -1, &rect, DT_NOCLIP);
        DeleteObject(hFont);

        EndPaint(hWnd, &ps);
        break;
    }

    }
    return 0;
}
*/

struct PixelFont {
    HFONT   fFont;
    LOGFONTA lFont;

	PixelFont(const char* name, int height)
	{
        //Logical units are device dependent pixels, so this will create a handle to a logical font that is 48 pixels in height.
        //The width, when set to 0, will cause the font mapper to choose the closest matching value.
        //The font face name will be Impact.
        lFont = {
            height,                 // cHeight
            0,                      // cWidth
            0,                      // cEscapement
            0,                      // cOrientation
            FW_DONTCARE,            // cWeight
            FALSE,                  // bItalic
            FALSE,                  // bUnderline
            FALSE,                  // bStrikeOut
            DEFAULT_CHARSET,        // iCharset
            OUT_OUTLINE_PRECIS,     // iOutPrecision
            CLIP_DEFAULT_PRECIS,    // iClipPrecision
            CLEARTYPE_QUALITY,      // iQuality
            VARIABLE_PITCH          // iPitchAndFamily
            };                  
        // faceName
        strncpy_s(lFont.lfFaceName, LF_FACESIZE, name, strlen(name));
        
        fFont = ::CreateFontIndirectA(&lFont);
	}

    ~PixelFont()
    {
        ::DeleteObject(fFont);
    }
};

void textFont(const char *fName, int size)
{
    PixelFont f1(fName, size);
    SelectObject(gAppSurface->getDC(), f1.fFont);
}

void text(const char* txt, double x, double y)
{
    ::TextOutA(gAppSurface->getDC(), x, y, txt, strlen(txt));
}

void textColor(COLORREF c)
{
    ::SetTextColor(gAppSurface->getDC(), c);
}

struct FontMonger {
    static int CALLBACK fontEnumProc(CONST LOGFONTA* font, CONST TEXTMETRICA* textMetric, DWORD fontType, LPARAM lParam)
    {
        std::list<std::string>* fontList = (std::list<std::string>*)lParam;
        fontList->push_back(std::string((char*)font->lfFaceName));

        return 1;
    }

    static void listFontFamilies()
    {
        LOGFONTA font;
        font.lfCharSet = DEFAULT_CHARSET;
        font.lfFaceName[0] = 0;
        font.lfPitchAndFamily = 0;

        std::list<std::string> fontList;

        int res = EnumFontFamiliesExA(gAppSurface->getDC(), &font, fontEnumProc, (LPARAM)&fontList, 0);
        //printf("EnumFontFamiliesExA: %d\n", res);

        fontList.unique();

        printf("===== UNIQUE NAMES =====\n");
        std::list<std::string>::iterator it;
        for (it = fontList.begin(); it != fontList.end(); ++it) {
            printf("%s\n", it->c_str());
        }
    }
};

void onFrame()
{
    static int fontSize = 12;
    static int dir = 1;

    background(0xffffffff);

    PixelRGBA c(0xff000000);
    textColor(c.toCOLORREF());
    //textFont("Arial", fontSize);
    //textFont("Yu Gothic UI", fontSize);
    textFont("Vivaldi", fontSize);
    text("The Quick brown fox jumps over the lazy dogs back.", 10, 100);



    if (fontSize >= 128)
        dir = -1;
    
    if (fontSize <= 10)
        dir = 1;

    fontSize += dir;
}



void setup()
{
    setFrameRate(30);
	setCanvasSize(1920, 480);

    FontMonger::listFontFamilies();
}