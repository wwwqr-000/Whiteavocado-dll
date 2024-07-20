#include <windows.h>
#include <iostream>
#include <fstream>
#include <string>

//For username
#include <Lmcons.h>
//

//For selfName
#include <shlwapi.h>
//

//For selectFolder
#include <shlobj.h>
//

//For quietShell
#include <tchar.h>
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

extern "C" int __cdecl moveSelfStartup(const char* path_, const char* sName_) {//Use a full path (From drive letter to folder with '/' at the end!)
    std::string self = getSelfName();
    std::string path = gsfcp(path_);
    std::string sName = gsfcp(sName_);
    if (isStartup()) { return 1; }//The current file is already processed by this code. (Alternate Data Stream detected)
    std::string pStartup = "C:/Users/" + std::string(getUName()) + "/AppData/Roaming/Microsoft/Windows/Start Menu/Programs/Startup/";
    std::string psCmd = "powershell.exe -Command \"& { Set-Location '" + pStartup + "'; $ws = New-Object -ComObject WScript.Shell; $s = $ws.CreateShortcut($pwd.Path + '\\" + sName + ".lnk'); $s.TargetPath = '" + path + self + "'; $s.WorkingDirectory = '" + path + "'; $s.Save() }\"";
    std::string oneToRuleThemAll = "@echo off && type \"" + self + "\" >> \"" + path + self + "\" && echo check > \"" + path + self + ":startup\" && cd \"" + pStartup + "\" && " + psCmd + " && echo check > \"" + pStartup + sName + ".lnk:startup\"";

    system(oneToRuleThemAll.c_str());

    return 0;
}

extern "C" void __cdecl quietShell(std::string command, std::string& result) {
    SECURITY_ATTRIBUTES saAttr;
    saAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
    saAttr.bInheritHandle = TRUE;
    saAttr.lpSecurityDescriptor = NULL;
    HANDLE hStdoutRd, hStdoutWr;
    CreatePipe(&hStdoutRd, &hStdoutWr, &saAttr, 0);
    _STARTUPINFOW si;
    ZeroMemory(&si, sizeof(_STARTUPINFOW));
    si.cb = sizeof(_STARTUPINFOW);
    si.dwFlags = STARTF_USESTDHANDLES;
    si.hStdOutput = hStdoutWr;
    si.hStdError = hStdoutWr;

    PROCESS_INFORMATION pi;
    ZeroMemory(&pi, sizeof(PROCESS_INFORMATION));

    std::wstring commandWString = std::wstring(command.begin(), command.end());
    std::wstring cmd = L"/c " + commandWString;
    TCHAR cmdPath[MAX_PATH];
    GetSystemDirectory(cmdPath, MAX_PATH);
    _stprintf(cmdPath, _T("%s\\cmd.exe"), cmdPath);
    std::wstring cmdPathWString(cmdPath, cmdPath + _tcslen(cmdPath));

    if (!CreateProcessW(cmdPathWString.c_str(), &cmd[0], NULL, NULL, TRUE, 0, NULL, NULL, &si, &pi)) {
        DWORD errorCode = GetLastError();
        LPSTR errorBuffer = nullptr;
        FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
                      NULL, errorCode, 0, (LPSTR)&errorBuffer, 0, NULL);
        std::cerr << "CreateProcess failed: " << errorBuffer << std::endl;
        LocalFree(errorBuffer);
        return;
    }
    CloseHandle(hStdoutWr);

    char buffer[1024];
    DWORD bytesRead;
    while (ReadFile(hStdoutRd, buffer, 1024, &bytesRead, NULL) && bytesRead > 0) {
        result.append(buffer, bytesRead);
    }

    size_t pos = result.rfind('\n');
    if (pos!= std::string::npos) {
        result.erase(pos);
    }

    CloseHandle(hStdoutRd);
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
}

extern "C" void __cdecl getTargetPath(std::string lnk, std::string& result) {
    quietShell("powershell.exe $lnkFile = '" + lnk + "'; $shell = New-Object -ComObject WScript.Shell; $shortcut = $shell.CreateShortcut($lnkFile); $targetPath = $shortcut.TargetPath; Write-Host $targetPath", result);
    for (char& c : result) {
        if (c == '\\') {
            c = '/';
        }
    }
}
