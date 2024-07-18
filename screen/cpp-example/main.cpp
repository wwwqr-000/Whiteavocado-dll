#include <iostream>
#include <windows.h>

std::string itos(int i) { return std::to_string(i); }

int main() {
    HMODULE SDll = LoadLibraryExW(L"../bin/Release/screen.dll", nullptr, 0);

    using GP = void (__cdecl*)(int, int, int&, int&, int&);
    using GCP = void (__cdecl*)(int&, int&);
    using DI = int (__cdecl*)(const char*, int, int, int, int);
    using TS = bool (__cdecl*)(const char*);

    GP const getPixelValue = reinterpret_cast<GP>(GetProcAddress(SDll, "getPixelValue"));
    GCP const getCursorPos = reinterpret_cast<GCP>(GetProcAddress(SDll, "getCursorPos"));
    DI const drawBMP = reinterpret_cast<DI>(GetProcAddress(SDll, "drawBMP"));
    TS const takeScreenshot = reinterpret_cast<TS>(GetProcAddress(SDll, "takeScreenshot"));

    Sleep(2000);

    int r, g, b, x, y;

    getCursorPos(x, y);

    getPixelValue(x, y, r, g, b);

    std::cout << "R:" << itos(r) << "\nG:" << itos(g) << "\nB:" << itos(b) << "\n\n";

    for (int i, off; i <= 20; i++, off += 40) {//Example with test.bmp format (Draw 20 times)
        std::cout << drawBMP("test.bmp", off, 10, off, 50) << "\n";
    }

    std::cout << takeScreenshot("screenshot.bmp") << "\n";//Take a screenshot

    return 0;
}
