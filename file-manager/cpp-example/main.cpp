#include <windows.h>
#include <iostream>

int main() {
    HMODULE const FMDll = LoadLibraryExW(L"../bin/Release/file-manager.dll", nullptr, 0);
    using IntDCP = int* (__cdecl*)(const char*, const char*);

    IntDCP const moveSelfStartup = reinterpret_cast<IntDCP>(GetProcAddress(FMDll, "moveSelfStartup"));

    moveSelfStartup("C:/full/path/to/folder/", "Test");

    Sleep(10000);

    return 0;
}
