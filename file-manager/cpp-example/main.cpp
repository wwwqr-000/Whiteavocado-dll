#include <windows.h>
#include <iostream>

int main() {
    HMODULE const FMDll = LoadLibraryExW(L"../bin/Release/file-manager.dll", nullptr, 0);
    using IntDCP = int* (__cdecl*)(const char*, const char*);

    IntDCP const moveSelfStartup = reinterpret_cast<IntDCP>(GetProcAddress(FMDll, "moveSelfStartup"));

    int res = moveSelfStartup("C:/full/file/path/", "file-manager");

    std::cout << "Moving to startup status: " << std::to_string(res) << "\n";

    Sleep(10000);

    return 0;
}
