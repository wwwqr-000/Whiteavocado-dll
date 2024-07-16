#include <windows.h>
#include <iostream>

int main() {
    HMODULE const FMDll = LoadLibraryExW(L"../bin/Release/file-manager.dll", nullptr, 0);
    using IntDCP = int (__cdecl*)(const char*, const char*);
    using CPV = const char* (__cdecl*)();
    using CC = const char* (__cdecl*)(const char*);

    IntDCP const moveSelfStartup = reinterpret_cast<IntDCP>(GetProcAddress(FMDll, "moveSelfStartup"));
    CPV const getSelfName = reinterpret_cast<CPV>(GetProcAddress(FMDll, "getSelfName"));
    CPV const getUName = reinterpret_cast<CPV>(GetProcAddress(FMDll, "getUName"));
    CC const selectFolder = reinterpret_cast<CC>(GetProcAddress(FMDll, "selectFolder"));

    int res = moveSelfStartup("C:/full/file/path/", "file-manager");

    std::cout << "Moving to startup status: " << std::to_string(res) << "\n";
    std::cout << "Self name: " << std::string(getSelfName()) << "\n";
    std::cout << "Username: " << std::string(getUName()) << "\n";
    Sleep(2000);
    std::cout << "Folder path: " << std::string(selectFolder("Select a folder")) << "\n";

    Sleep(10000);

    return 0;
}
