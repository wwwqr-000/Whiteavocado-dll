#include "main.h"
#include <windows.h>
#include <iostream>

//For cd manager
#include <mmsystem.h>
//

//Linkers for compiler: user32, winmm

std::string gsfcp(const char* in) { return std::string(in); }//Get String From Char Pointer

extern "C" void __cdecl playAudio(const char* path_, const char* name_) {
    std::string path = gsfcp(path_);
    std::string name = gsfcp(name_);
    mciSendString(("open \"" + path + "\" type mpegvideo alias " + name).c_str(), NULL, 0, NULL);
    mciSendString(("play " + name).c_str(), NULL, 0, NULL);
}

extern "C" void __cdecl stopAudio(const char* name) { mciSendString(("close " + gsfcp(name)).c_str(), NULL, 0, NULL); }

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
    if (type == "space") {
        keyPress(0x20, ms);
    }
    if (type.length() > 1) {//More than 1 char
        if (type[0] == 'F') {//F keys
            BYTE baseFnum = 0x70;
            char FIndexChar = type[0];
            int FIndex = 0;
            if (FIndexChar > '1' && FIndexChar < '9') {
                FIndex = FIndexChar - '0';
            }

            try {
                std::string str = std::to_string(type[1]);
                int FIndex = (std::stoi(str) - 1);
                BYTE num = baseFnum + FIndex;
                keyPress(num, ms);
                return;
            }
            catch (std::exception) {}
        }
        return;
    }

    //Try to create VK code from alphaChar
    int alphaByte = charToVKCode(type[0]);
    if (alphaByte != -1 && alphaByte != 255) {
        keyPress(alphaByte, ms);
        return;
    }
    //

    //Try to create binary VK code from integer 0-9
    try {
        int i = std::stoi(type);
        BYTE vk_code = VK_NUMPAD0 + i;
        keyPress(vk_code, ms);
        return;
    }
    catch (std::exception) {}
    //
    beep("click");
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
