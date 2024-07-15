#include <windows.h>
#include <iostream>

LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode == HC_ACTION) {
        switch (wParam) {
            case WM_KEYDOWN:
                std::cout << "Key down: " << (int)((KBDLLHOOKSTRUCT*)lParam)->vkCode << std::endl;
            break;
            case WM_KEYUP:
                std::cout << "Key up: " << (int)((KBDLLHOOKSTRUCT*)lParam)->vkCode << std::endl;
            break;
        }
    }

    return CallNextHookEx(NULL, nCode, wParam, lParam);
}

extern "C" void __cdecl key(const char* type, bool debug) {
    HHOOK hHook = SetWindowsHookEx(WH_KEYBOARD, KeyboardProc, GetModuleHandle(NULL), 0);
    if (hHook == NULL && debug) { std::cout << "Could not start listener.\n"; return; }

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    UnhookWindowsHookEx(hHook);
}
