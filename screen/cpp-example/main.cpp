#include <iostream>
#include <windows.h>

std::string itos(int i) { return std::to_string(i); }

int main() {
    HMODULE SDll = LoadLibraryExW(L"../bin/Release/screen.dll", nullptr, 0);

    using GP = void (__cdecl*)(int, int, int&, int&, int&);
    using GCP = void (__cdecl*)(int&, int&);

    GP const getPixelValue = reinterpret_cast<GP>(GetProcAddress(SDll, "getPixelValue"));
    GCP const getCursorPos = reinterpret_cast<GCP>(GetProcAddress(SDll, "getCursorPos"));

    Sleep(2000);

    int r, g, b, x, y;

    getCursorPos(x, y);

    getPixelValue(x, y, r, g, b);

    std::cout << "R:" << itos(r) << "\nG:" << itos(g) << "\nB:" << itos(b) << "\n";

    return 0;
}
