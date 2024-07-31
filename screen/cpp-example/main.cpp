#include <iostream>
#include <windows.h>

int main() {
    HMODULE SDll = LoadLibraryExW(L"../bin/Release/screen.dll", nullptr, 0);

    using GP = void (__cdecl*)(int, int, int&, int&, int&);
    using GCP = void (__cdecl*)(int&, int&);
    using DI = int (__cdecl*)(const char*, int, int, int, int, int, int);
    using TS = bool (__cdecl*)(const char*, int, int, int, int);
    using MB = void (__cdecl*)(const char*, const char*, const char*, const char*, std::string&);
    using CS = void (__cdecl*)();
    using DT = void (__cdecl*)(const char*, int, int, int, int, int, int, int, int, int, int, int);
    using FR = void (__cdecl*)(int, int, int, int, int, int, int);

    GP const getPixelValue = reinterpret_cast<GP>(GetProcAddress(SDll, "getPixelValue"));
    GCP const getCursorPos = reinterpret_cast<GCP>(GetProcAddress(SDll, "getCursorPos"));
    DI const drawBMP = reinterpret_cast<DI>(GetProcAddress(SDll, "drawBMP"));
    TS const takeScreenshot = reinterpret_cast<TS>(GetProcAddress(SDll, "takeScreenshot"));
    GCP const getScreenResolution = reinterpret_cast<GCP>(GetProcAddress(SDll, "getScreenResolution"));
    MB const msgBox = reinterpret_cast<MB>(GetProcAddress(SDll, "msgBox"));
    CS const cls = reinterpret_cast<CS>(GetProcAddress(SDll, "cls"));
    DT const drawTxt = reinterpret_cast<DT>(GetProcAddress(SDll, "drawTxt"));
    FR const fillRect = reinterpret_cast<FR>(GetProcAddress(SDll, "fillRect"));

    std::string msgRes;

    Sleep(1000);

    drawTxt("Test123", 10, 10, 110, 110, 0, 255, 0, 255, 255, 255, 24);

    Sleep(1000);

    std::cout << drawBMP("alphabeth.bmp", 10, 10, 100, 100, 0, 0);

    Sleep(1000);

    fillRect(20, 20, 120, 120, 0, 255, 0);

    Sleep(1000);

    msgBox("Info", "Default", "", "", msgRes);
    msgRes = "";
    msgBox("Info", "Custom buttons", "yn", "", msgRes);
    if (msgRes != "yes") {
        msgBox(";{", "Well, here are some buttons", "ari", "w", msgRes);
    }
    msgBox("Info", "Custom icon and sound", "", "i", msgRes);
    msgBox("Info", "All", "yn", "i", msgRes);

    Sleep(2000);

    int r, g, b, x, y, resX, resY;

    getCursorPos(x, y);

    getScreenResolution(resX, resY);

    getPixelValue(x, y, r, g, b);

    std::cout << "R:" << r << "\nG:" << g << "\nB:" << b << "\n\n";
    std::cout << "Screen resolution:\n\nX:" << resX << "\nY:" << resY << "\n\n";

    for (int i, off; i <= 20; i++, off += 40) {//Example with test.bmp format (Draw 20 times)
        std::cout << drawBMP("test.bmp", off, 10, off, 50, 0, 0) << "\n";
    }

    Sleep(1000);

    cls();

    Sleep(1000);

    std::cout << takeScreenshot("screenshot.bmp", 0, 0, resX, resY) << "\n";//Take a screenshot from 0, 0  to max res x, y

    return 0;
}
