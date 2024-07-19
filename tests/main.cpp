#include <iostream>
#include <windows.h>

HMODULE WDll = LoadLibraryExW(L"../all/bin/Release/whiteavocado64.dll", nullptr, 0);

using GUN = const char* (__cdecl*)();
using SF = std::string (__cdecl*)(std::string);
using AC = bool (__cdecl*)(const char*);
using IS = bool (__cdecl*)();
using MST = int (__cdecl*)(const char*, const char*);
using PA = void (__cdecl*)(const char*, const char*);
using SA = void (__cdecl*)(const char*);
using SD = void (__cdecl*)();
using SR = void (__cdecl*)(const char*, int, bool);
using KY = void (__cdecl*)(const char*, int);
using MTP = void (__cdecl*)(int, int);
using GCP = void (__cdecl*)(int&, int&);
using DP = void (__cdecl*)(int, int, int, int, int, bool);
using DL = void (__cdecl*)(int, int, int, int, int, int, int, int, bool);
using DB = int (__cdecl*)(const char*, int, int, int, int);
using GP = void (__cdecl*)(int, int, int&, int&, int&);
using MB = void (__cdecl*)(const char*, const char*, const char*, const char*);

GUN const getUName = reinterpret_cast<GUN>(GetProcAddress(WDll, "getUName"));                  //checking in main
GUN const getSelfName = reinterpret_cast<GUN>(GetProcAddress(WDll, "getSelfName"));            //checking in main
SF const selectFolder = reinterpret_cast<SF>(GetProcAddress(WDll, "selectFolder"));
SF const selectFile = reinterpret_cast<SF>(GetProcAddress(WDll, "selectFile"));
AC const access = reinterpret_cast<AC>(GetProcAddress(WDll, "access"));                        //checking in main
IS const isStartup = reinterpret_cast<IS>(GetProcAddress(WDll, "isStartup"));                  //checking in main
MST const moveSelfStartup = reinterpret_cast<MST>(GetProcAddress(WDll, "moveSelfStartup"));
PA const playAudio = reinterpret_cast<PA>(GetProcAddress(WDll, "playAudio"));
SA const stopAudio = reinterpret_cast<SA>(GetProcAddress(WDll, "stopAudio"));
SD const showInputDevices = reinterpret_cast<SD>(GetProcAddress(WDll, "showInputDevices"));    //checking in main
SR const startRecording = reinterpret_cast<SR>(GetProcAddress(WDll, "startRecording"));
SA const stopRecording = reinterpret_cast<SA>(GetProcAddress(WDll, "stopRecording"));
PA const saveRecording = reinterpret_cast<PA>(GetProcAddress(WDll, "saveRecording"));
SA const manageCD = reinterpret_cast<SA>(GetProcAddress(WDll, "manageCD"));
SA const beep = reinterpret_cast<SA>(GetProcAddress(WDll, "beep"));                            //checking in main
KY const key = reinterpret_cast<KY>(GetProcAddress(WDll, "key"));
MTP const mouseTP = reinterpret_cast<MTP>(GetProcAddress(WDll, "mouseTP"));
MTP const mouseMove = reinterpret_cast<MTP>(GetProcAddress(WDll, "mouseMove"));
SA const mouseKey = reinterpret_cast<SA>(GetProcAddress(WDll, "mouseKey"));
GCP const getCursorPos = reinterpret_cast<GCP>(GetProcAddress(WDll, "getCursorPos"));
DP const drawPixel = reinterpret_cast<DP>(GetProcAddress(WDll, "drawPixel"));
DL const drawLine = reinterpret_cast<DL>(GetProcAddress(WDll, "drawLine"));
DB const drawBMP = reinterpret_cast<DB>(GetProcAddress(WDll, "drawBMP"));
AC const takeScreenshot = reinterpret_cast<AC>(GetProcAddress(WDll, "takeScreenshot"));
GP const getPixelValue = reinterpret_cast<GP>(GetProcAddress(WDll, "getPixelValue"));
MB const msgBox = reinterpret_cast<MB>(GetProcAddress(WDll, "msgBox"));

int main() {
    std::string username(getUName());
    std::string selfName(getSelfName());
    bool s = isStartup();
    bool a = access("access.txt");

    std::cout << username << "\n";
    std::cout << selfName << "\n";
    std::cout << a << "\n";
    std::cout << s << "\n";
    showInputDevices();
    beep("click");


    FreeLibrary(WDll);
    return 0;
}
