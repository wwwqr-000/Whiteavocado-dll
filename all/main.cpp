#include <windows.h>
#include <iostream>
#include <fstream>

//Compiler linkers: -s -static-libstdc++ -static-libgcc -static  -luser32 -lshlwapi -lole32 -lcomdlg32 -lwinmm -lgdi32

//For username
#include <Lmcons.h>
//

//For selfName
#include <shlwapi.h>
//

//For selectFolder
#include <shlobj.h>
//

//For cd manager and recording
#include <mmsystem.h>
//

//For quietShell
#include <tchar.h>
//

//For recording -> volume level
#include <mmdeviceapi.h>
#include <endpointvolume.h>
//

std::string gsfcp(const char* s) { return std::string(s); }

//file-manager.dll
bool access_(std::string path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        return false;
    }
    file.close();
    return true;
}

extern "C" const char* __cdecl getUName() {
    WCHAR username[UNLEN + 1];
    DWORD username_len = UNLEN + 1;
    GetUserNameW(username, &username_len);

    int requiredSize = WideCharToMultiByte(CP_UTF8, 0, username, -1, nullptr, 0, nullptr, nullptr);
    char* narrowUsername = new char[requiredSize];
    WideCharToMultiByte(CP_UTF8, 0, username, -1, narrowUsername, requiredSize, nullptr, nullptr);

    return narrowUsername;
}

extern "C" const char* __cdecl getSelfName() {
    static std::string fn;
    TCHAR filename[MAX_PATH];
    GetModuleFileName(NULL, filename, MAX_PATH);
    TCHAR *filename_only = PathFindFileName(filename);
    fn = filename_only;
    return fn.c_str();
}

extern "C" std::string __cdecl selectFolder(std::string description) {
    TCHAR szDir[MAX_PATH];

    BROWSEINFO bi;
    ZeroMemory(&bi, sizeof(bi));
    bi.hwndOwner = NULL;
    bi.ulFlags = BIF_NEWDIALOGSTYLE | BIF_RETURNONLYFSDIRS;
    std::string descriptionA = description;
    bi.lpszTitle = descriptionA.c_str();
    LPITEMIDLIST pidl = SHBrowseForFolder(&bi);

    if (!pidl) { return "error"; }

    SHGetPathFromIDList(pidl, szDir);
    CoTaskMemFree(pidl);

    for (char& c : szDir) {
        if (c == '\\') {
            c = '/';
        }
    }

    std::string path = szDir;
    if (path.back() != '/') {
        path += "/";
    }
    return path;
}

extern "C" std::string __cdecl selectFile(std::string description) {
    std::string fPath;
    OPENFILENAME ofn;
    char szFile[MAX_PATH] = "";

    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = NULL;
    ofn.lpstrFile = szFile;
    ofn.nMaxFile = MAX_PATH;
    ofn.lpstrTitle = description.c_str();
    ofn.Flags = OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;

    if (GetOpenFileName(&ofn)) {
        for (char& c : szFile) {
            if (c == '\\') {
                c = '/';
            }
        }
        std::string fPath(szFile);
        return fPath;
    }
    return "error";
}

extern "C" bool __cdecl access(const char* filePath) { return access_(gsfcp(filePath)); }
extern "C" bool __cdecl isStartup() { return (access_(std::string(getSelfName()) + ":startup")); }

extern "C" bool __cdecl quietShell(std::string command, std::string& result) {
    SECURITY_ATTRIBUTES saAttr;
    saAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
    saAttr.bInheritHandle = TRUE;
    saAttr.lpSecurityDescriptor = NULL;
    HANDLE hStdoutRd, hStdoutWr;
    CreatePipe(&hStdoutRd, &hStdoutWr, &saAttr, 0);
    _STARTUPINFOW si;
    ZeroMemory(&si, sizeof(_STARTUPINFOW));
    si.cb = sizeof(_STARTUPINFOW);
    si.dwFlags = STARTF_USESTDHANDLES;
    si.hStdOutput = hStdoutWr;
    si.hStdError = hStdoutWr;

    PROCESS_INFORMATION pi;
    ZeroMemory(&pi, sizeof(PROCESS_INFORMATION));

    std::wstring commandWString = std::wstring(command.begin(), command.end());
    std::wstring cmd = L"/c " + commandWString;
    TCHAR cmdPath[MAX_PATH];
    GetSystemDirectory(cmdPath, MAX_PATH);
    _stprintf(cmdPath, _T("%s\\cmd.exe"), cmdPath);
    std::wstring cmdPathWString(cmdPath, cmdPath + _tcslen(cmdPath));

    if (!CreateProcessW(cmdPathWString.c_str(), &cmd[0], NULL, NULL, TRUE, CREATE_NO_WINDOW, NULL, NULL, &si, &pi)) {
        return false;
    }
    CloseHandle(hStdoutWr);

    char buffer[1024];
    DWORD bytesRead;
    while (ReadFile(hStdoutRd, buffer, 1024, &bytesRead, NULL) && bytesRead > 0) {
        result.append(buffer, bytesRead);
    }

    size_t pos = result.rfind('\n');
    if (pos!= std::string::npos) {
        result.erase(pos);
    }

    CloseHandle(hStdoutRd);
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
    return true;
}

extern "C" int __cdecl moveSelfStartup(const char* path_, const char* sName_) {//Use a full path (From drive letter to folder with '/' at the end!)
    std::string self = getSelfName();
    std::string path = gsfcp(path_);
    std::string sName = gsfcp(sName_);
    std::string qsResult;
    if (isStartup()) { return 1; }//The current file is already processed by this code. (Alternate Data Stream detected)
    std::string pStartup = "C:/Users/" + std::string(getUName()) + "/AppData/Roaming/Microsoft/Windows/Start Menu/Programs/Startup/";
    std::string psCmd = "powershell.exe -Command \"& { Set-Location '" + pStartup + "'; $ws = New-Object -ComObject WScript.Shell; $s = $ws.CreateShortcut($pwd.Path + '\\" + sName + ".lnk'); $s.TargetPath = '" + path + self + "'; $s.WorkingDirectory = '" + path + "'; $s.Save() }\"";
    std::string oneToRuleThemAll = "@echo off && type \"" + self + "\" >> \"" + path + self + "\" && echo check > \"" + path + self + ":startup\" && cd \"" + pStartup + "\" && " + psCmd + " && echo check > \"" + pStartup + sName + ".lnk:startup\"";

    quietShell(oneToRuleThemAll, qsResult);

    return 0;
}

extern "C" void __cdecl getTargetPath(std::string lnk, std::string& result) {
    quietShell("powershell.exe $lnkFile = '" + lnk + "'; $shell = New-Object -ComObject WScript.Shell; $shortcut = $shell.CreateShortcut($lnkFile); $targetPath = $shortcut.TargetPath; Write-Host $targetPath", result);
    for (char& c : result) {
        if (c == '\\') {
            c = '/';
        }
    }
}

//

//hardware-emulator.dll
extern "C" void __cdecl playAudio(const char* path_, const char* name_) {
    std::string path = gsfcp(path_);
    std::string name = gsfcp(name_);
    mciSendString(("open \"" + path + "\" type mpegvideo alias " + name).c_str(), NULL, 0, NULL);
    mciSendString(("play " + name).c_str(), NULL, 0, NULL);
}

extern "C" void __cdecl stopAudio(const char* name) { mciSendString(("close " + gsfcp(name)).c_str(), NULL, 0, NULL); }

extern "C" void __cdecl showInputDevices() {
    UINT numDevices = waveInGetNumDevs();
    std::cout << "Number of input devices (microphones): " << numDevices << std::endl;

    for (UINT i = 0; i < numDevices; ++i) {
        WAVEINCAPS waveCaps;
        MMRESULT result = waveInGetDevCaps(i, &waveCaps, sizeof(WAVEINCAPS));
        if (result == MMSYSERR_NOERROR) {
            std::cout << "Input Device #" << i << ": " << waveCaps.szPname << std::endl;
        }
    }
}

bool setRecordingVolume(float volumeLevel, bool debug) {
    HRESULT hr = CoInitialize(NULL);
    if (FAILED(hr) && debug) {
        std::cout << "Failed to initialize COM library\n";
        return false;
    }

    IMMDeviceEnumerator* deviceEnumerator = NULL;
    hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_INPROC_SERVER, __uuidof(IMMDeviceEnumerator), (LPVOID*)&deviceEnumerator);
    if (FAILED(hr) && debug) {
        std::cout << "Failed to create device enumerator\n";
        CoUninitialize();
        return false;
    }

    IMMDevice* defaultDevice = NULL;
    hr = deviceEnumerator->GetDefaultAudioEndpoint(eCapture, eConsole, &defaultDevice);
    deviceEnumerator->Release();
    if (FAILED(hr) && debug) {
        std::cout << "Failed to get default audio endpoint\n";
        CoUninitialize();
        return false;
    }

    IAudioEndpointVolume* endpointVolume = NULL;
    hr = defaultDevice->Activate(__uuidof(IAudioEndpointVolume), CLSCTX_INPROC_SERVER, NULL, (LPVOID*)&endpointVolume);
    defaultDevice->Release();
    if (FAILED(hr) && debug) {
        std::cout << "Failed to activate audio endpoint volume\n";
        CoUninitialize();
        return false;
    }

    hr = endpointVolume->SetMasterVolumeLevelScalar(volumeLevel, NULL);
    endpointVolume->Release();
    CoUninitialize();

    if (FAILED(hr) && debug) {
        std::cout << "Failed to set master volume level\n";
        return false;
    }

    return true;
}

extern "C" void __cdecl startRecording(const char* name, int inputDeviceIndex, bool debug = false) {
    WAVEINCAPSA waveInCaps;
    MMRESULT result = waveInGetDevCaps(inputDeviceIndex, &waveInCaps, sizeof(WAVEINCAPSA));
    if (result!= MMSYSERR_NOERROR && debug) {
        std::cout << "Failed to get wave format for input device #" << inputDeviceIndex << "\n";
        return;
    }

    WAVEFORMATEX waveFormat;
    // Check the supported formats
    if (waveInCaps.dwFormats && WAVE_FORMAT_PCM) {
        // Device supports 48 kHz, 24-bit, stereo
        waveFormat.wFormatTag = WAVE_FORMAT_PCM;
        waveFormat.nChannels = 2; // Stereo
        waveFormat.nSamplesPerSec = 48000; // 48 kHz
        waveFormat.nAvgBytesPerSec = 288000; // 288000 bytes per second for 48 kHz, 24-bit, stereo
        waveFormat.nBlockAlign = 6; // 6 bytes per sample (24-bit, stereo)
        waveFormat.wBitsPerSample = 24; // 24-bit samples
    }
    else if (waveInCaps.dwFormats && WAVE_FORMAT_44S16) {
        // Device supports 44.1 kHz, 16-bit, stereo
        waveFormat.wFormatTag = WAVE_FORMAT_PCM;
        waveFormat.nChannels = 2; // Stereo
        waveFormat.nSamplesPerSec = 44100; // 44.1 kHz
        waveFormat.nAvgBytesPerSec = 176400; // 176400 bytes per second for 44.1 kHz, 16-bit, stereo
        waveFormat.nBlockAlign = 4; // 4 bytes per sample (16-bit, stereo)
        waveFormat.wBitsPerSample = 16; // 16-bit samples
    }
    else if (waveInCaps.dwFormats && WAVE_FORMAT_1M08) {
        // Device supports 11.025 kHz, 8-bit, mono
        waveFormat.wFormatTag = WAVE_FORMAT_PCM;
        waveFormat.nChannels = 1; // Mono
        waveFormat.nSamplesPerSec = 11025; // 11.025 kHz
        waveFormat.nAvgBytesPerSec = 11025; // 11025 bytes per second for 11.025 kHz, 8-bit, mono
        waveFormat.nBlockAlign = 1; // 1 byte per sample (8-bit, mono)
        waveFormat.wBitsPerSample = 8; // 8-bit samples
    }
    else {
        if (debug) {
            std::cout << "Device does not support any known formats\n";
        }
        return;
    }
    //

    //Set recording volume level
    if (!setRecordingVolume(1.0, debug) && debug) {
        std::cout << "Failed to set recording volume. Skipping...\n";
    }
    //

    mciSendString(("open new type waveaudio alias " + gsfcp(name)).c_str(), NULL, 0, NULL);
    mciSendString(("record " + gsfcp(name)).c_str(), NULL, 0, NULL);
}

extern "C" void __cdecl stopRecording(const char* name) {
    mciSendString(("stop " + gsfcp(name)).c_str(), NULL, 0, NULL);
}

extern "C" void __cdecl saveRecording(const char* name, const char* path) {
    mciSendString(("save " + gsfcp(name) + " \"" + gsfcp(path) + "\"").c_str(), NULL, 0, NULL);
    mciSendString(("close " + gsfcp(name)).c_str(), NULL, 0, NULL);
}

char getCDDriveLetter() {
    char drive = '\0';
    DWORD drives = GetLogicalDrives();

    for (char i = 'A'; i <= 'Z'; i++) {
        if (drives & (1 << (i - 'A'))) {
            std::string rootPath = std::string(1, i) + ":\\";
            UINT driveType = GetDriveType(rootPath.c_str());
            if (driveType == DRIVE_CDROM) {
                drive = i;
                break;
            }
        }
    }
    return drive;
}

extern "C" void __cdecl manageCD(const char* status_) {
    std::string driveLetter = std::to_string(getCDDriveLetter());
    std::string status = gsfcp(status_);
    if (status == "open") {
        std::string command = "open " + driveLetter + ": type CDAudio alias cd";
        mciSendString(command.c_str(), NULL, 0, NULL);
        mciSendString("set cd door open", NULL, 0, NULL);
    }
    else if (status == "close") {
        mciSendString("set cd door closed", NULL, 0, NULL);
        mciSendString("close cd", NULL, 0, NULL);
    }
}

extern "C" void __cdecl beep(const char* type) {
    std::string strType = gsfcp(type);
    if (strType == "normal") {
        MessageBeep(MB_ICONASTERISK);
    }
    else if (strType == "strong") {
        MessageBeep(MB_ICONEXCLAMATION);
    }
    else if (strType == "crit") {
        MessageBeep(MB_ICONHAND);
    }
    else if (strType == "click") {
        Beep(1000, 10);
    }
}

void keyPress(BYTE b, int ms) {
    keybd_event(b, 0, 0, 0);
    Sleep(ms);
    keybd_event(b, 0, KEYEVENTF_KEYUP, 0);
}

BYTE charToVKCode(char in) {
    BYTE A = 0x41;
    if (in >= 'a' && in <= 'z') {
        return A + (in - 'a');
    }
    else if (in >= 'A' && in <= 'Z') {
        return A + (in - 'A');
    }
    return -1;
}

extern "C" void __cdecl key(const char* type_, int ms = 10) {
    std::string type = gsfcp(type_);
    //Special cases
    if (type == "space") { keyPress(0x20, ms); return; }
    if (type == "esc") { keyPress(0x1B, ms); return; }
    if (type == "tab") { keyPress(0x09, ms); return; }
    if (type == "caps") { keyPress(0x3A, ms); return; }
    if (type == "shift") { keyPress(0x10, ms); return; }
    if (type == "alt-left") { keyPress(0x12, ms); return; }
    if (type == "alt-right") { keyPress(0x13, ms); return; }
    if (type == "win") { keyPress(0x5B, ms); return; }
    if (type == "enter") { keyPress(0x0D, ms); return; }
    if (type == "backsp") { keyPress(0x08, ms); return; }
    if (type == ".") { keyPress(0x2E, ms); return; }
    if (type == ",") { keyPress(0x2C, ms); return; }
    if (type == ">") { keyPress(0x3E, ms); return; }
    if (type == "<") { keyPress(0x3C, ms); return; }
    //

    //Arrow cases
    if (type.find("arrow") == 0) {
        if (type == "arrow-up")    { keyPress(0x48, ms); return; }
        if (type == "arrow-down")  { keyPress(0x50, ms); return; }
        if (type == "arrow-left")  { keyPress(0x4B, ms); return; }
        if (type == "arrow-right") { keyPress(0x4D, ms); return; }
    }
    //

    //Try to create VK code from alphaChar
    int alphaByte = charToVKCode(type[0]);
    if (alphaByte != -1 && alphaByte != 255) {
        keyPress(alphaByte, ms);
        return;
    }
    //

    if (type.length() > 1) {//More than 1 char
        if (type[0] == 'F' || type[0] == 'f') {
            //Check if it's a valid F-key format
            try {
                std::stoi(std::to_string(type[1]));
            }
            catch (std::exception) { return; }
            //
            if (type == "F1") { keyPress(0x70, ms); return; }
            if (type == "F2") { keyPress(0x71, ms); return; }
            if (type == "F3") { keyPress(0x72, ms); return; }
            if (type == "F4") { keyPress(0x73, ms); return; }
            if (type == "F5") { keyPress(0x74, ms); return; }
            if (type == "F6") { keyPress(0x75, ms); return; }
            if (type == "F7") { keyPress(0x76, ms); return; }
            if (type == "F8") { keyPress(0x77, ms); return; }
            if (type == "F9") { keyPress(0x78, ms); return; }
            if (type == "F10") { keyPress(0x79, ms); return; }
            if (type == "F11") { keyPress(0x7A, ms); return; }
            if (type == "F12") { keyPress(0x7B, ms); return; }
            if (type == "F13") { keyPress(0x7C, ms); return; }
            if (type == "F14") { keyPress(0x7D, ms); return; }
            if (type == "F15") { keyPress(0x7E, ms); return; }
            if (type == "F16") { keyPress(0x7F, ms); return; }
            if (type == "F17") { keyPress(0x80, ms); return; }
            if (type == "F18") { keyPress(0x81, ms); return; }
            if (type == "F19") { keyPress(0x82, ms); return; }
            if (type == "F20") { keyPress(0x83, ms); return; }
            if (type == "F21") { keyPress(0x84, ms); return; }
            if (type == "F22") { keyPress(0x85, ms); return; }
            if (type == "F23") { keyPress(0x86, ms); return; }
            if (type == "F24") { keyPress(0x86, ms); return; }
        }
    }

    //Try to create binary VK code from integer 0-9
    try {
        int i = std::stoi(type);
        BYTE vk_code = VK_NUMPAD0 + i;
        keyPress(vk_code, ms);
    }
    catch (std::exception) {}
    //
}

extern "C" void __cdecl mouseTP(int x, int y) {
    SetCursorPos(x, y);
}

extern "C" void __cdecl mouseMove(int offsetX, int offsetY) {
    POINT pt;
    GetCursorPos(&pt);
    int steps = 1000;

    for (int i = 0; i < steps; i++) {
        int currX = pt.x + (offsetX * i) / steps;
        int currY = pt.y + (offsetY * i) / steps;
        mouse_event(MOUSEEVENTF_MOVE | MOUSEEVENTF_ABSOLUTE, currX, currY, 0, 0);
    }
}

extern "C" void __cdecl mouseKey(const char* btn_) {
    std::string btn = gsfcp(btn_);
    POINT pt;
    GetCursorPos(&pt);

    if (btn == "left_down") { mouse_event(MOUSEEVENTF_LEFTDOWN, pt.x, pt.y, 0, 0); return; }
    if (btn == "left_up") { mouse_event(MOUSEEVENTF_LEFTUP, pt.x, pt.y, 0, 0); return; }
    if (btn == "right_down") { mouse_event(MOUSEEVENTF_RIGHTDOWN, pt.x, pt.y, 0, 0); return; }
    if (btn == "right_up") { mouse_event(MOUSEEVENTF_RIGHTUP, pt.x, pt.y, 0, 0); return; }
    if (btn == "middle_down") { mouse_event(MOUSEEVENTF_MIDDLEDOWN, pt.x, pt.y, 0, 0); return; }
    if (btn == "middle_up") { mouse_event(MOUSEEVENTF_MIDDLEUP, pt.x, pt.y, 0, 0); return; }
}
//

//screen.dll
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

extern "C" int __cdecl drawBMP(const char* imgPath, int x, int y, int width, int height, int offX, int offY) {
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

    BitBlt(hdcScreen, x, y, width, height, hdcMem, offX, offY, SRCCOPY);

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

extern "C" void __cdecl drawTxt(const char* txt, int beginX, int beginY, int endX, int endY, int tR, int tG, int tB, int bR, int bG, int bB, int fontSize) {
    RECT rect = { beginX, beginY, endX, endY };
    HDC hdc = GetDC(NULL);
    int fontSizeInPixels = -MulDiv(fontSize, GetDeviceCaps(hdc, LOGPIXELSY), 72);
    HFONT font = CreateFont(fontSizeInPixels, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "Arial");
    HFONT oldFont = (HFONT)SelectObject(hdc, font);
    SetTextColor(hdc, RGB(tR, tG, tB));
    SetBkColor(hdc, RGB(bR, bG, bB));
    SetBkMode(hdc, OPAQUE);
    DrawText(hdc, txt, -1, &rect, DT_SINGLELINE | DT_NOCLIP);
    SelectObject(hdc, oldFont);
    DeleteObject(font);
    ReleaseDC(NULL, hdc);
}

extern "C" void __cdecl fillRect(int beginX, int beginY, int endX, int endY, int r, int g, int b) {
    HDC hdc = GetDC(NULL);
    RECT box = { beginX, beginY, endX, endY };
    HBRUSH hBrush = CreateSolidBrush(RGB(r, g, b));

    FillRect(hdc, &box, hBrush);
    DeleteObject(hBrush);
    ReleaseDC(NULL, hdc);
}
//end screen.dll

//Listener
bool lStat = true;
bool bStat = true;
int triggeredKeyVK;
int buttonIndex;

LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {
    bool keyPressed = false;
    if (nCode == HC_ACTION) {
        KBDLLHOOKSTRUCT* pKB = (KBDLLHOOKSTRUCT*)lParam;
        switch (wParam) {
            case WM_KEYDOWN:
                if (!keyPressed) {
                    triggeredKeyVK = (int)pKB->vkCode;
                    keyPressed = true;
                }
                break;
            case WM_KEYUP:
                keyPressed = false;
                lStat = false;
                break;
        }
    }
    return CallNextHookEx(NULL, nCode, wParam, lParam);
}

LRESULT CALLBACK MouseHookProc(int nCode, WPARAM wParam, LPARAM lParam) {
    bool valid = true;
    if (nCode != HC_ACTION) {
        return CallNextHookEx(NULL, nCode, wParam, lParam);
    }
    switch (wParam) {
        case WM_LBUTTONDOWN:
            buttonIndex = 0;
        break;
        case WM_RBUTTONDOWN:
            buttonIndex = 1;
        break;
        case WM_MBUTTONDOWN:
            buttonIndex = 2;
        break;
        case WM_LBUTTONUP:
            buttonIndex = 3;
        break;
        case WM_RBUTTONUP:
            buttonIndex = 4;
        break;
        case WM_MBUTTONUP:
            buttonIndex = 5;
        break;
        default:
            valid = false;
    }

    if (valid) {
        bStat = false;
    }

    return CallNextHookEx(NULL, nCode, wParam, lParam);
}

extern "C" int __cdecl keyListener(std::string& stat) {
    lStat = true;
    HMODULE hModule = GetModuleHandle(NULL);
    HHOOK hHook = SetWindowsHookEx(WH_KEYBOARD_LL, KeyboardProc, hModule, 0);
    if (hHook == NULL) {
        DWORD errorCode = GetLastError();
        stat = ("Could not start listener. Error: " + std::to_string(errorCode) + "\n").c_str();
        return 0;
    }

    MSG msg;
    while (lStat) {
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }
    UnhookWindowsHookEx(hHook);
    stat = "ok";
    return triggeredKeyVK;
}

extern "C" int __cdecl buttonListener(std::string& stat) {
    bStat = true;
    HMODULE hModule = GetModuleHandle(NULL);
    HHOOK hMouseHook = SetWindowsHookEx(WH_MOUSE_LL, MouseHookProc, hModule, 0);
    if (hMouseHook == NULL) {
        stat = "Failed to setup hook";
        return -1;
    }
    MSG msg;
    while (bStat) {
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    UnhookWindowsHookEx(hMouseHook);
    return buttonIndex;
}
//
