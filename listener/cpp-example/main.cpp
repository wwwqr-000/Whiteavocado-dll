#include <iostream>
#include <windows.h>

HMODULE LDll = LoadLibraryExW(L"../bin/Release/listener.dll", nullptr, 0);

using KL = char (__cdecl*)(const char*, std::string&);

KL const keyListener = reinterpret_cast<KL>(GetProcAddress(LDll, "keyListener"));


void free() {
    FreeLibrary(LDll);
}

int main() {
    std::string kyResult;
    while (true) {
        std::cout << keyListener("global", kyResult);
        kyResult = "";
    }
    free();
    return 0;
}
