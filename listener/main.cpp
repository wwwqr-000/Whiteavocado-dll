#include <windows.h>
#include <iostream>

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
    if (nCode != HC_ACTION) {
        return CallNextHookEx(NULL, nCode, wParam, lParam);
    }
    switch (wParam) {
        case WM_LBUTTONDOWN:
            buttonIndex = 0;
            bStat = false;
        break;
        case WM_RBUTTONDOWN:
            buttonIndex = 1;
            bStat = false;
        break;
        case WM_MBUTTONDOWN:
            buttonIndex = 2;
            bStat = false;
        break;
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
