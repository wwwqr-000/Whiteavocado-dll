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

extern "C" void __cdecl msgBox(const char* title, const char* innerTxt, const char* button_, const char* icon_, const char* result) {
    int MB_icon, MB_button;
    std::string button = std::string(button_);
    std::string icon = std::string(icon_);

    if (button == "ari") {
        MB_button = MB_ABORTRETRYIGNORE;
    }
    else if (button == "ctc") {
        MB_button = MB_CANCELTRYCONTINUE;
    }
    else if (button == "h") {
        MB_button = MB_HELP;
    }
    else if (button == "o") {
        MB_button = MB_OK;
    }
    else if (button == "oc") {
        MB_button = MB_OKCANCEL;
    }
    else if (button == "rc") {
        MB_button = MB_RETRYCANCEL;
    }
    else if (button == "yn") {
        MB_button = MB_YESNO;
    }
    else if (button == "ync") {
        MB_button = MB_YESNOCANCEL;
    }

    if (icon == "!") {
        MB_icon = MB_ICONEXCLAMATION;
    }
    else if (icon == "w") {
        MB_icon = MB_ICONWARNING;
    }
    else if (icon == "i") {
        MB_icon = MB_ICONINFORMATION;
    }
    else if (icon == "a") {
        MB_icon = MB_ICONASTERISK;
    }
    else if (icon == "q") {
        MB_icon = MB_ICONQUESTION;
    }
    else if (icon == "s") {
        MB_icon = MB_ICONSTOP;
    }
    else if (icon == "e") {
        MB_icon = MB_ICONERROR;
    }

    int resVal = MessageBox(NULL, innerTxt, title, MB_icon | MB_button);

    switch (resVal) {
        case 1:
            result = "ok";
        break;
        case 2:
            result = "cancel";
        break;
        case 3:
            result = "abort";
        break;
        case 4:
            result = "retry";
        break;
        case 5:
            result = "ignore";
        break;
        case 6:
            result = "yes";
        break;
        case 7:
            result = "no";
        break;
        case 8:
            result = "close";
        break;
        case 9:
            result = "help";
        break;
        case 10:
            result = "tryagain";
        break;
        case 11:
            result = "continue";
        break;
        default:
            result = "empty";
    }
}
