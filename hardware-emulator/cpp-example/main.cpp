#include <iostream>
#include <windows.h>

int main() {
    HMODULE const HEDll = LoadLibraryExW(L"../bin/Release/hardware-emulator.dll", nullptr, 0);
    using VoidType = void* (__cdecl*)();
    using GCType = void* (__cdecl*)(int&, int&);
    using MTType = void* (__cdecl*)(int, int);
    using keyType = void* (__cdecl*)(const char*);

    VoidType const showInputDevices = reinterpret_cast<VoidType>(GetProcAddress(HEDll, "showInputDevices"));
    GCType const getCursorPos = reinterpret_cast<GCType>(GetProcAddress(HEDll, "getCursorPos"));
    MTType const mouseTP = reinterpret_cast<MTType>(GetProcAddress(HEDll, "mouseTP"));
    keyType const key = reinterpret_cast<keyType>(GetProcAddress(HEDll, "key"));
    MTType const mouseMove = reinterpret_cast<MTType>(GetProcAddress(HEDll, "mouseMove"));

    Sleep(5000);
    showInputDevices();
    int x, y;
    getCursorPos(x, y);
    std::cout << "x: " <<  std::to_string(x) << "\ny: " << std::to_string(y) << "\n";
    mouseTP(10, 10);

    FreeLibrary(HEDll);
    return 0;
}
