#include "main.h"
#include <windows.h>
#include <iostream>

#include <windows.h>
#include <iostream>

extern "C" void __cdecl beep(const char* type) {
    std::string strType = std::string(type);
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

void keyPress(BYTE b) {
    keybd_event(b, 0, 0, 0);
    keybd_event(b, 0, KEYEVENTF_KEYUP, 0);
}

BYTE intToByte(int in) { return (byte)in; }
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

extern "C" void __cdecl key(const char* type_) {
    std::string type = std::string(type_);
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
                keyPress(num);
                return;
            }
            catch (std::exception) {}
        }
        return;
    }

    //Try to create VK code from alphaChar
    int alphaByte = charToVKCode(type[0]);
    if (alphaByte != -1 && alphaByte != 255) {
        keyPress(alphaByte);
        return;
    }
    //

    //Try to create binary VK code from integer 0-9
    try {
        int i = std::stoi(type);
        BYTE vk_code = VK_NUMPAD0 + i;
        keyPress(vk_code);
        return;
    }
    catch (std::exception) {}
    //
    beep("click");
}

extern "C" void __cdecl mouse(const char* btn_) {
    std::string btn = std::string(btn_);
    POINT pt;
    GetCursorPos(&pt);

    if (btn == "left_down") { mouse_event(MOUSEEVENTF_LEFTDOWN, pt.x, pt.y, 0, 0); return; }
    if (btn == "left_up") { mouse_event(MOUSEEVENTF_LEFTUP, pt.x, pt.y, 0, 0); return; }
    if (btn == "right_down") { mouse_event(MOUSEEVENTF_RIGHTDOWN, pt.x, pt.y, 0, 0); return; }
    if (btn == "right_up") { mouse_event(MOUSEEVENTF_RIGHTUP, pt.x, pt.y, 0, 0); return; }
    if (btn == "middle_down") { mouse_event(MOUSEEVENTF_MIDDLEDOWN, pt.x, pt.y, 0, 0); return; }
    if (btn == "middle_up") { mouse_event(MOUSEEVENTF_MIDDLEUP, pt.x, pt.y, 0, 0); return; }
}
