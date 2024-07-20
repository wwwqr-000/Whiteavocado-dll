#include <windows.h>
#include <iostream>

int main() {
    HMODULE const FMDll = LoadLibraryExW(L"../bin/Release/file-manager.dll", nullptr, 0);
    using IntDCP = int (__cdecl*)(const char*, const char*);
    using CPV = const char* (__cdecl*)();
    using CC = std::string (__cdecl*)(std::string);
    using GTP = void (__cdecl*)(std::string, std::string&);
    using QS = bool (__cdecl*)(std::string, std::string&);

    IntDCP const moveSelfStartup = reinterpret_cast<IntDCP>(GetProcAddress(FMDll, "moveSelfStartup"));
    CPV const getSelfName = reinterpret_cast<CPV>(GetProcAddress(FMDll, "getSelfName"));
    CPV const getUName = reinterpret_cast<CPV>(GetProcAddress(FMDll, "getUName"));
    CC const selectFolder = reinterpret_cast<CC>(GetProcAddress(FMDll, "selectFolder"));
    CC const selectFile = reinterpret_cast<CC>(GetProcAddress(FMDll, "selectFile"));
    QS const quietShell = reinterpret_cast<QS>(GetProcAddress(FMDll, "quietShell"));

    std::string shellRes;
    quietShell("echo hello there!", shellRes);
    std::cout << shellRes << "\n";

    int res = moveSelfStartup("C:/full/file/path/", "file-manager");

    std::cout << "Moving to startup status: " << std::to_string(res) << "\n";
    std::cout << "Self name: " << getSelfName() << "\n";
    std::cout << "Username: " << std::string(getUName()) << "\n";
    Sleep(2000);
    std::cout << "Folder path: " << selectFolder("Select a folder") << "\n";
    std::cout << "File path: " << selectFile("Select a file") << "\n";

    Sleep(10000);

    FreeLibrary(FMDll);
    return 0;
}
