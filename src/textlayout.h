#pragma once

#include "apphost.h"

// Reference
// https://docs.microsoft.com/en-us/windows/win32/directwrite/introducing-directwrite
// https://docs.microsoft.com/en-us/windows/win32/directwrite/getting-started-with-directwrite
// 
// Text rendering
// 	// Text Alignment
enum class ALIGNMENT : unsigned
{
	CENTER = 0x01,

	LEFT = 0x02,
	RIGHT = 0x04,

	BASELINE = 0x20,
	TOP = 0x10,
	BOTTOM = 0x40,

};


struct PixelFont
{

public:
    HFONT   fFont;
    LOGFONTA lFont;

    PixelFont()
        :fFont(NULL)
        , lFont{}
    {
        fFont = NULL;
    }

    PixelFont(const char* name, int height)
    {
        init(name, height);

        //Logical units are device dependent pixels, so this will create a handle to a logical font that is height pixels tall.
        //The width, when set to 0, will cause the font mapper to choose the closest matching value.
        //The font face name will be 'name'.
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
        //reset();
    }

    INLINE void init(const char* name, int height)
    {
        // first clearout whatever is there already
        reset();

        //Logical units are device dependent pixels, so this will create a handle to a logical font that is height pixels tall.
//The width, when set to 0, will cause the font mapper to choose the closest matching value.
//The font face name will be 'name'.
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

        strncpy_s(lFont.lfFaceName, LF_FACESIZE, name, strlen(name));

        fFont = ::CreateFontIndirectA(&lFont);
    }

    INLINE void reset()
    {
        ::DeleteObject(fFont);
    }
};

struct TextLayout 
{
    std::shared_ptr<User32PixelMap> fSurface;

    PixelFont gCurrentFont;     // object for currently selected font
    TEXTMETRICA gTextMetric;

	// Font globals
	int fontHeight;
	int fontAscent;
	int fontDescent;
	int fontLeading;


    ALIGNMENT fTextHAlignment;
    ALIGNMENT fTextVAlignment;

    TextLayout()
    {
        fTextHAlignment = ALIGNMENT::LEFT;
        fTextVAlignment = ALIGNMENT::BASELINE;
    }

    void init(std::shared_ptr<User32PixelMap> surface)
    {
        fSurface = surface;
    }

	INLINE void textAlign(ALIGNMENT horizontal, ALIGNMENT vertical)
	{
        fTextHAlignment = horizontal;
        fTextVAlignment = vertical;
	}

	INLINE void textFont(const char* fName, int size)
	{
        gCurrentFont.init(fName, size);
        ::SelectObject(gAppSurface->getDC(), gCurrentFont.fFont);

        ::GetTextMetricsA(gAppSurface->getDC(), &gTextMetric);
        fontHeight = gTextMetric.tmHeight;
        fontAscent = gTextMetric.tmAscent;
        fontDescent = gTextMetric.tmDescent;
        fontLeading = gTextMetric.tmInternalLeading;
	}

	INLINE void text(const char* txt, double x, double y)
	{
        PixelCoord pt;
        calcTextPosition(pt, txt, x, y);

        ::TextOutA(gAppSurface->getDC(), pt.x(), pt.y(), txt, strlen(txt));
	}

	INLINE void textColor(const PixelRGBA& pix)
	{
        auto c = pix.toCOLORREF();
        ::SetTextColor(gAppSurface->getDC(), c);
	}

	INLINE void textMeasure(PixelCoord& pt, const char* txt)
	{
        SIZE sz;
        ::GetTextExtentPoint32A(gAppSurface->getDC(), txt, strlen(txt), &sz);

        pt[0] = sz.cx;
        pt[1] = sz.cy;
	}


    // Here we are crudely doing a minimal amount necessary
    // to see some text.  We should be using FontMonger to cache
    // font information so we're not reading it every time.
    void calcTextPosition(PixelCoord& pt, const char* txt, double x, double y)
    {
        PixelCoord txtSize;
        textMeasure(txtSize, txt);
        double cx = txtSize.x();
        double cy = txtSize.y();

        switch (fTextHAlignment)
        {
        case ALIGNMENT::LEFT:
            x = x;
            break;
        case ALIGNMENT::CENTER:
            x = x - (cx / 2);
            break;
        case ALIGNMENT::RIGHT:
            x = x - cx;
            break;
        }

        switch (fTextVAlignment)
        {
        case ALIGNMENT::TOP:
            y = y;
            break;
        case ALIGNMENT::CENTER:
            y = y - (cy / 2);
            break;
        case ALIGNMENT::BASELINE:
            y = y-fontAscent;
            break;
        case ALIGNMENT::BOTTOM:
            y = y-fontHeight;
            break;
        }

        pt[0] = x;
        pt[1] = y;
    }

};
