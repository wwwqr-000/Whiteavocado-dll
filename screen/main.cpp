#include <windows.h>
#include <iostream>

//Compiler parameters: -luser32 -lgdi32

extern "C" void __cdecl drawPixel(int x, int y, int r, int g, int b, bool debug) {
    HDC hdc = CreateDC("DISPLAY", NULL, NULL, NULL);
    if (hdc == NULL && debug) { std::cout << "Could not draw pixel: hdc not be set.\n"; return; }

    COLORREF color = RGB(r, g, b);
    SetPixel(hdc, x, y, color);
    if (debug) { std::cout << "Drew pixel.\n"; }
    DeleteDC(hdc);
}

extern "C" void __cdecl drawLine(int xStart, int yStart, int xEnd, int yEnd, int width, int r, int g, int b, bool debug) {
    HDC hdc = CreateDC("DISPLAY", NULL, NULL, NULL);
    COLORREF color = RGB(r, g, b);
    HPEN hPen = CreatePen(PS_SOLID, width, color);
    HPEN oldPen = (HPEN)SelectObject(hdc, hPen);
    MoveToEx(hdc, xStart, yStart, NULL);
    LineTo(hdc, xEnd, yEnd);

    SelectObject(hdc, oldPen);
    DeleteObject(hPen);
    DeleteDC(hdc);
    if (debug) { std::cout << "Drew line.\n"; }
}

extern "C" void __cdecl cls() {
    std::cout << "No working cls at the time. (W.I.P)\n";
}

extern "C" void __cdecl getCursorPos(int& x, int& y) {
    POINT pt;
    GetCursorPos(&pt);
    x = pt.x;
    y = pt.y;
}
