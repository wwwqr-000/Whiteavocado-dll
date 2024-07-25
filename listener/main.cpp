#include <windows.h>
#include <iostream>

bool lStat = true;
int triggeredKeyVK;

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

extern "C" int __cdecl keyListener(const char* type, std::string& stat) {
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
