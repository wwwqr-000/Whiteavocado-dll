#include <iostream>
#include <windows.h>

HMODULE LDll = LoadLibraryExW(L"../bin/Release/listener.dll", nullptr, 0);

using BL = int (__cdecl*)(std::string&);

BL const keyListener = reinterpret_cast<BL>(GetProcAddress(LDll, "keyListener"));
BL const buttonListener = reinterpret_cast<BL>(GetProcAddress(LDll, "buttonListener"));


void free() {
    FreeLibrary(LDll);
}

int main() {
    std::string kyResult;
    while (true) {
        std::cout << buttonListener(kyResult);
        kyResult = "";
        std::cout << keyListener(kyResult);
        kyResult = "";
    }
    free();
    return 0;
}
