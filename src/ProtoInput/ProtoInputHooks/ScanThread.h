#pragma once
//#include "HandleMainWindow.h"


namespace ScreenshotInput {

    class ScanThread {
    public:
        static CRITICAL_SECTION critical; //window thread
        static int scanAtype, scanBtype, scanXtype, scanYtype, Ctype, Dtype, Etype, Ftype;
        static int numphotoA, numphotoB, numphotoX, numphotoY, numphotoC, numphotoD, numphotoE, numphotoF;
        static int numphotoAbmps, numphotoBbmps, numphotoXbmps, numphotoYbmps;
        static bool UpdateWindow;
        static POINT PointA, PointB, PointX, PointY;
        static int startsearchA, startsearchB, startsearchX, startsearchY, startsearchC, startsearchD, startsearchE, startsearchF;
        static std::vector<POINT> staticPointA, staticPointB, staticPointX, staticPointY;
        static bool scanloop;
        //tunell
        static int Aisstatic, Bisstatic, Xisstatic, Yisstatic;
        static bool scanoption;
        static bool ShoulderNextBMP;

        static int resize;
        static int ignorerect;
        static void StartScanThread(HMODULE hmodule, int Astatic, int Bstatic, int Xstatic, int Ystatic, bool prescan);

        static bool SaveWindow10x10BMP(HWND hwnd, std::wstring filename, int x, int y);
        static bool enumeratebmps(); //false if no bmps found
        static bool initovector();
        static bool ButtonPressed(UINT buttonFlag);
    };
}