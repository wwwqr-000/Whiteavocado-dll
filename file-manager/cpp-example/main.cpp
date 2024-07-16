#include <windows.h>
#include <iostream>

int main() {
    HMODULE const FMDll = LoadLibraryExW(L"../bin/Release/file-manager.dll", nullptr, 0);
    using IntDCP = int (__cdecl*)(const char*, const char*);
    using CPV = const char* (__cdecl*)();

    IntDCP const moveSelfStartup = reinterpret_cast<IntDCP>(GetProcAddress(FMDll, "moveSelfStartup"));
    CPV const getSelfName = reinterpret_cast<CPV>(GetProcAddress(FMDll, "getSelfName"));
    CPV const getUName = reinterpret_cast<CPV>(GetProcAddress(FMDll, "getUName"));

    int res = moveSelfStartup("C:/full/file/path/", "file-manager");

    std::cout << "Moving to startup status: " << std::to_string(res) << "\n";
    std::cout << "Self name: " << std::string(getSelfName()) << "\n";
    std::cout << "Username: " << std::string(getUName()) << "\n";

    Sleep(10000);

    return 0;
}
