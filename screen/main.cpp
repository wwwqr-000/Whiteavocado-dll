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

extern "C" int __cdecl drawBMP(const char* imgPath, int x, int y, int width, int height) {
    HBITMAP hBitmap = (HBITMAP)LoadImage(NULL, imgPath, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
    if (!hBitmap) { return 1; }

    HDC hdcScreen = GetDC(NULL);
    if (!hdcScreen) {
        DeleteObject(hBitmap);
        return 2;
    }

    BITMAP bm;
    int retVal = GetObject(hBitmap, sizeof(BITMAP), &bm);
    if (retVal != sizeof(BITMAP)) {
        DeleteObject(hBitmap);
        ReleaseDC(NULL, hdcScreen);
        return 3;
    }

    HDC hdcMem = CreateCompatibleDC(hdcScreen);
    if (!hdcMem) {
        DeleteObject(hBitmap);
        ReleaseDC(NULL, hdcScreen);
        return 4;
    }

    HGDIOBJ oldBitmap = SelectObject(hdcMem, hBitmap);
    if (!oldBitmap) {
        DeleteDC(hdcMem);
        DeleteObject(hBitmap);
        ReleaseDC(NULL, hdcScreen);
        return 5;
    }

    BitBlt(hdcScreen, x, y, width, height, hdcMem, 0, 0, SRCCOPY);

    SelectObject(hdcMem, oldBitmap);
    DeleteDC(hdcMem);
    DeleteObject(hBitmap);
    ReleaseDC(NULL, hdcScreen);

    return 0;
}

extern "C" void getScreenResolution(int& x, int& y) {
    RECT box;
    GetWindowRect(GetDesktopWindow(), &box);
    x = box.right;
    y = box.bottom;
}

extern "C" bool __cdecl takeScreenshot(const char* fullOutPath, int beginX, int beginY, int endX, int endY) {
    if (beginX >= endX || beginY >= endY) {//There is nothing to capture in that space.
        return false;
    }
    int width = endX - beginX;
    int height = endY - beginY;

    HDC hdc = GetDC(NULL);
    HDC hdcMem = CreateCompatibleDC(hdc);
    HBITMAP bitmap = CreateCompatibleBitmap(hdc, width, height);
    SelectObject(hdcMem, bitmap);

    BitBlt(hdcMem, 0, 0, width, height, hdc, beginX, beginY, SRCCOPY);

    BITMAPFILEHEADER bfHeader;
    BITMAPINFOHEADER biHeader;
    biHeader.biSize = sizeof(BITMAPINFOHEADER);
    biHeader.biWidth = width;
    biHeader.biHeight = height;
    biHeader.biPlanes = 1;
    biHeader.biBitCount = 24;
    biHeader.biCompression = BI_RGB;
    biHeader.biSizeImage = ((width * 3 + 3) & ~3) * height;
    biHeader.biXPelsPerMeter = 0;
    biHeader.biYPelsPerMeter = 0;
    biHeader.biClrUsed = 0;
    biHeader.biClrImportant = 0;

    bfHeader.bfType = 0x4D42;//BMP
    bfHeader.bfSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + biHeader.biSizeImage;
    bfHeader.bfReserved1 = 0;
    bfHeader.bfReserved2 = 0;
    bfHeader.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);

    HANDLE file = CreateFileA(fullOutPath, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (file == INVALID_HANDLE_VALUE) {
        DeleteObject(bitmap);
        DeleteDC(hdcMem);
        ReleaseDC(NULL, hdc);
        return false;
    }

    DWORD bytesWritten;
    WriteFile(file, &bfHeader, sizeof(BITMAPFILEHEADER), &bytesWritten, NULL);
    WriteFile(file, &biHeader, sizeof(BITMAPINFOHEADER), &bytesWritten, NULL);

    unsigned char* pixels = new unsigned char[biHeader.biSizeImage];
    GetDIBits(hdcMem, bitmap, 0, height, pixels, (BITMAPINFO*)&biHeader, DIB_RGB_COLORS);

    WriteFile(file, pixels, biHeader.biSizeImage, &bytesWritten, NULL);
    delete[] pixels;
    CloseHandle(file);

    DeleteObject(bitmap);
    DeleteDC(hdcMem);
    ReleaseDC(NULL, hdc);

    return true;
}

extern "C" void __cdecl cls() {
    InvalidateRect(NULL, NULL, TRUE);
    UpdateWindow(NULL);
}

extern "C" void __cdecl getCursorPos(int& x, int& y) {
    POINT pt;
    GetCursorPos(&pt);
    x = pt.x;
    y = pt.y;
}

extern "C" void __cdecl getPixelValue(int x, int y, int& r, int& g, int& b) {
    HDC hdc = GetDC(NULL);
    COLORREF color = GetPixel(hdc, x, y);
    ReleaseDC(NULL, hdc);

    r = GetRValue(color);
    g = GetGValue(color);
    b = GetBValue(color);
}

extern "C" void __cdecl msgBox(const char* title, const char* innerTxt, const char* button_, const char* icon_, std::string& result) {
    int MB_icon, MB_button;
    INT_PTR resVal;
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
    else {
        MB_button = MB_OK;
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
    else {
        icon = "none";
    }

    if (icon == "none") {
        resVal = MessageBox(NULL, innerTxt, title, MB_button);
    }
    else {
        resVal = MessageBox(NULL, innerTxt, title, MB_icon | MB_button);
    }

    switch (resVal) {
        case IDOK:
            result = "ok";
        break;
        case IDCANCEL:
            result = "cancel";
        break;
        case IDABORT:
            result = "abort";
        break;
        case IDRETRY:
            result = "retry";
        break;
        case IDIGNORE:
            result = "ignore";
        break;
        case IDYES:
            result = "yes";
        break;
        case IDNO:
            result = "no";
        break;
        default:
            result = "empty";
    }
}
