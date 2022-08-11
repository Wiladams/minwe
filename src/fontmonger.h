#pragma once


#include "wingdi.h"

#include <list>

struct FontMonger {
    // Callback used for Windows font enumeration function
    static int CALLBACK fontEnumProc(CONST LOGFONTA* font, CONST TEXTMETRICA* textMetric, DWORD fontType, LPARAM lParam)
    {
        std::list<std::string>* fontList = (std::list<std::string>*)lParam;
        fontList->push_back(std::string((char*)font->lfFaceName));

        return 1;
    }

    // Collect the names of the font families
    static void collectFontFamilies(std::list<std::string>& fontList)
    {
        LOGFONTA font;
        font.lfCharSet = DEFAULT_CHARSET;
        font.lfFaceName[0] = 0;
        font.lfPitchAndFamily = 0;

        int res = EnumFontFamiliesExA(gAppSurface->getDC(), &font, fontEnumProc, (LPARAM)&fontList, 0);

        fontList.unique();
    }

    // Print out the names of the font families
    static void listFontFamilies()
    {
        std::list<std::string> fontList;
        collectFontFamilies(fontList);

        printf("===== UNIQUE NAMES =====\n");
        std::list<std::string>::iterator it;
        for (it = fontList.begin(); it != fontList.end(); ++it) {
            printf("%s\n", it->c_str());
        }
    }
};
