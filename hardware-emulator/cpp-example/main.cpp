#include <iostream>
#include <windows.h>

int main() {
    HMODULE const HEDll = LoadLibraryExW(L"../bin/Release/hardware-emulator.dll", nullptr, 0);
    using VoidType = void* (__cdecl*)();
    using GCType = void* (__cdecl*)(int&, int&);
    using MTType = void* (__cdecl*)(int, int);
    using KeyType = void* (__cdecl*)(const char*, int ms);
    using CType = void* (__cdecl*)(const char*);
    using DCType = void* (__cdecl*)(const char*, const char*);

    VoidType const showInputDevices = reinterpret_cast<VoidType>(GetProcAddress(HEDll, "showInputDevices"));
    GCType const getCursorPos = reinterpret_cast<GCType>(GetProcAddress(HEDll, "getCursorPos"));
    MTType const mouseTP = reinterpret_cast<MTType>(GetProcAddress(HEDll, "mouseTP"));
    KeyType const key = reinterpret_cast<KeyType>(GetProcAddress(HEDll, "key"));
    MTType const mouseMove = reinterpret_cast<MTType>(GetProcAddress(HEDll, "mouseMove"));
    CType const manageCD = reinterpret_cast<CType>(GetProcAddress(HEDll, "manageCD"));
    DCType const playAudio = reinterpret_cast<DCType>(GetProcAddress(HEDll, "playAudio"));
    CType const stopAudio = reinterpret_cast<CType>(GetProcAddress(HEDll, "stopAudio"));
    CType const beep = reinterpret_cast<CType>(GetProcAddress(HEDll, "beep"));

    Sleep(5000);
    showInputDevices();
    int x, y;
    getCursorPos(x, y);
    std::cout << "x: " <<  std::to_string(x) << "\ny: " << std::to_string(y) << "\n";
    mouseTP(10, 10);
    manageCD("open");
    Sleep(1000);
    manageCD("close");
    playAudio("../python-example/rick.mp3", "rickroll");
    Sleep(5000);
    stopAudio("rickroll");
    beep("click");
    Sleep(2000);
    key("win", 10);
    Sleep(750);
    key("x", 10);
    key("d", 10);
    key("space", 10);
    key("<", 10);
    key(">", 10);
    Sleep(100);
    key("enter", 10);

    FreeLibrary(HEDll);
    return 0;
}
