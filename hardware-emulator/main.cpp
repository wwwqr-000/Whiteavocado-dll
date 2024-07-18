#include "main.h"
#include <windows.h>
#include <iostream>

//For cd manager and recording
#include <mmsystem.h>
//

//For recording -> volume level
#include <mmdeviceapi.h>
#include <endpointvolume.h>
//

//Compiler arguments used:
// -s -static-libstdc++ -static-libgcc -static  -luser32 -lwinmm -lole32

std::string gsfcp(const char* in) { return std::string(in); }//Get String From Char Pointer

extern "C" void __cdecl playAudio(const char* path_, const char* name_) {
    std::string path = gsfcp(path_);
    std::string name = gsfcp(name_);
    mciSendString(("open \"" + path + "\" type mpegvideo alias " + name).c_str(), NULL, 0, NULL);
    mciSendString(("play " + name).c_str(), NULL, 0, NULL);
}

extern "C" void __cdecl stopAudio(const char* name) { mciSendString(("close " + gsfcp(name)).c_str(), NULL, 0, NULL); }

extern "C" void __cdecl showInputDevices() {
    UINT numDevices = waveInGetNumDevs();
    std::cout << "Number of input devices (microphones): " << numDevices << std::endl;

    for (UINT i = 0; i < numDevices; ++i) {
        WAVEINCAPS waveCaps;
        MMRESULT result = waveInGetDevCaps(i, &waveCaps, sizeof(WAVEINCAPS));
        if (result == MMSYSERR_NOERROR) {
            std::cout << "Input Device #" << i << ": " << waveCaps.szPname << std::endl;
        }
    }
}

bool setRecordingVolume(float volumeLevel, bool debug) {
    HRESULT hr = CoInitialize(NULL);
    if (FAILED(hr) && debug) {
        std::cout << "Failed to initialize COM library\n";
        return false;
    }

    IMMDeviceEnumerator* deviceEnumerator = NULL;
    hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_INPROC_SERVER, __uuidof(IMMDeviceEnumerator), (LPVOID*)&deviceEnumerator);
    if (FAILED(hr) && debug) {
        std::cout << "Failed to create device enumerator\n";
        CoUninitialize();
        return false;
    }

    IMMDevice* defaultDevice = NULL;
    hr = deviceEnumerator->GetDefaultAudioEndpoint(eCapture, eConsole, &defaultDevice);
    deviceEnumerator->Release();
    if (FAILED(hr) && debug) {
        std::cout << "Failed to get default audio endpoint\n";
        CoUninitialize();
        return false;
    }

    IAudioEndpointVolume* endpointVolume = NULL;
    hr = defaultDevice->Activate(__uuidof(IAudioEndpointVolume), CLSCTX_INPROC_SERVER, NULL, (LPVOID*)&endpointVolume);
    defaultDevice->Release();
    if (FAILED(hr) && debug) {
        std::cout << "Failed to activate audio endpoint volume\n";
        CoUninitialize();
        return false;
    }

    hr = endpointVolume->SetMasterVolumeLevelScalar(volumeLevel, NULL);
    endpointVolume->Release();
    CoUninitialize();

    if (FAILED(hr) && debug) {
        std::cout << "Failed to set master volume level\n";
        return false;
    }

    return true;
}

extern "C" void __cdecl startRecording(const char* name, int inputDeviceIndex, bool debug = false) {
    WAVEINCAPSA waveInCaps;
    MMRESULT result = waveInGetDevCaps(inputDeviceIndex, &waveInCaps, sizeof(WAVEINCAPSA));
    if (result!= MMSYSERR_NOERROR && debug) {
        std::cout << "Failed to get wave format for input device #" << inputDeviceIndex << "\n";
        return;
    }

    WAVEFORMATEX waveFormat;
    // Check the supported formats
    if (waveInCaps.dwFormats && WAVE_FORMAT_PCM) {
        // Device supports 48 kHz, 24-bit, stereo
        waveFormat.wFormatTag = WAVE_FORMAT_PCM;
        waveFormat.nChannels = 2; // Stereo
        waveFormat.nSamplesPerSec = 48000; // 48 kHz
        waveFormat.nAvgBytesPerSec = 288000; // 288000 bytes per second for 48 kHz, 24-bit, stereo
        waveFormat.nBlockAlign = 6; // 6 bytes per sample (24-bit, stereo)
        waveFormat.wBitsPerSample = 24; // 24-bit samples
    }
    else if (waveInCaps.dwFormats && WAVE_FORMAT_44S16) {
        // Device supports 44.1 kHz, 16-bit, stereo
        waveFormat.wFormatTag = WAVE_FORMAT_PCM;
        waveFormat.nChannels = 2; // Stereo
        waveFormat.nSamplesPerSec = 44100; // 44.1 kHz
        waveFormat.nAvgBytesPerSec = 176400; // 176400 bytes per second for 44.1 kHz, 16-bit, stereo
        waveFormat.nBlockAlign = 4; // 4 bytes per sample (16-bit, stereo)
        waveFormat.wBitsPerSample = 16; // 16-bit samples
    }
    else if (waveInCaps.dwFormats && WAVE_FORMAT_1M08) {
        // Device supports 11.025 kHz, 8-bit, mono
        waveFormat.wFormatTag = WAVE_FORMAT_PCM;
        waveFormat.nChannels = 1; // Mono
        waveFormat.nSamplesPerSec = 11025; // 11.025 kHz
        waveFormat.nAvgBytesPerSec = 11025; // 11025 bytes per second for 11.025 kHz, 8-bit, mono
        waveFormat.nBlockAlign = 1; // 1 byte per sample (8-bit, mono)
        waveFormat.wBitsPerSample = 8; // 8-bit samples
    }
    else {
        if (debug) {
            std::cout << "Device does not support any known formats\n";
        }
        return;
    }
    //

    //Set recording volume level
    if (!setRecordingVolume(1.0, debug) && debug) {
        std::cout << "Failed to set recording volume. Skipping...\n";
    }
    //

    mciSendString(("open new type waveaudio alias " + gsfcp(name)).c_str(), NULL, 0, NULL);
    mciSendString(("record " + gsfcp(name)).c_str(), NULL, 0, NULL);
}

extern "C" void __cdecl stopRecording(const char* name) {
    mciSendString(("stop " + gsfcp(name)).c_str(), NULL, 0, NULL);
}

extern "C" void __cdecl saveRecording(const char* name, const char* path) {
    mciSendString(("save " + gsfcp(name) + " \"" + gsfcp(path) + "\"").c_str(), NULL, 0, NULL);
    mciSendString(("close " + gsfcp(name)).c_str(), NULL, 0, NULL);
}

char getCDDriveLetter() {
    char drive = '\0';
    DWORD drives = GetLogicalDrives();

    for (char i = 'A'; i <= 'Z'; i++) {
        if (drives & (1 << (i - 'A'))) {
            std::string rootPath = std::string(1, i) + ":\\";
            UINT driveType = GetDriveType(rootPath.c_str());
            if (driveType == DRIVE_CDROM) {
                drive = i;
                break;
            }
        }
    }
    return drive;
}

extern "C" void __cdecl manageCD(const char* status_) {
    std::string driveLetter = std::to_string(getCDDriveLetter());
    std::string status = gsfcp(status_);
    if (status == "open") {
        std::string command = "open " + driveLetter + ": type CDAudio alias cd";
        mciSendString(command.c_str(), NULL, 0, NULL);
        mciSendString("set cd door open", NULL, 0, NULL);
    }
    else if (status == "close") {
        mciSendString("set cd door closed", NULL, 0, NULL);
        mciSendString("close cd", NULL, 0, NULL);
    }
}

extern "C" void __cdecl beep(const char* type) {
    std::string strType = gsfcp(type);
    if (strType == "normal") {
        MessageBeep(MB_ICONASTERISK);
    }
    else if (strType == "strong") {
        MessageBeep(MB_ICONEXCLAMATION);
    }
    else if (strType == "crit") {
        MessageBeep(MB_ICONHAND);
    }
    else if (strType == "click") {
        Beep(1000, 10);
    }
}

void keyPress(BYTE b, int ms) {
    keybd_event(b, 0, 0, 0);
    Sleep(ms);
    keybd_event(b, 0, KEYEVENTF_KEYUP, 0);
}

BYTE charToVKCode(char in) {
    BYTE A = 0x41;
    if (in >= 'a' && in <= 'z') {
        return A + (in - 'a');
    }
    else if (in >= 'A' && in <= 'Z') {
        return A + (in - 'A');
    }
    return -1;
}

extern "C" void __cdecl key(const char* type_, int ms = 10) {
    std::string type = gsfcp(type_);
    //Special cases
    if (type == "space") { keyPress(0x20, ms); return; }
    if (type == "esc") { keyPress(0x1B, ms); return; }
    if (type == "tab") { keyPress(0x09, ms); return; }
    if (type == "caps") { keyPress(0x3A, ms); return; }
    if (type == "shift") { keyPress(0x10, ms); return; }
    if (type == "alt-left") { keyPress(0x12, ms); return; }
    if (type == "alt-right") { keyPress(0x13, ms); return; }
    if (type == "win") { keyPress(0x5B, ms); return; }
    if (type == "enter") { keyPress(0x0D, ms); return; }
    if (type == "backsp") { keyPress(0x08, ms); return; }
    if (type == ".") { keyPress(0x2E, ms); return; }
    if (type == ",") { keyPress(0x2C, ms); return; }
    if (type == ">") { keyPress(0x3E, ms); return; }
    if (type == "<") { keyPress(0x3C, ms); return; }
    //

    //Arrow cases
    if (type.find("arrow") == 0) {
        if (type == "arrow-up")    { keyPress(0x48, ms); return; }
        if (type == "arrow-down")  { keyPress(0x50, ms); return; }
        if (type == "arrow-left")  { keyPress(0x4B, ms); return; }
        if (type == "arrow-right") { keyPress(0x4D, ms); return; }
    }
    //

    //Try to create VK code from alphaChar
    int alphaByte = charToVKCode(type[0]);
    if (alphaByte != -1 && alphaByte != 255) {
        keyPress(alphaByte, ms);
        return;
    }
    //

    if (type.length() > 1) {//More than 1 char
        if (type[0] == 'F' || type[0] == 'f') {
            //Check if it's a valid F-key format
            try {
                std::stoi(std::to_string(type[1]));
            }
            catch (std::exception) { return; }
            //
            if (type == "F1") { keyPress(0x70, ms); return; }
            if (type == "F2") { keyPress(0x71, ms); return; }
            if (type == "F3") { keyPress(0x72, ms); return; }
            if (type == "F4") { keyPress(0x73, ms); return; }
            if (type == "F5") { keyPress(0x74, ms); return; }
            if (type == "F6") { keyPress(0x75, ms); return; }
            if (type == "F7") { keyPress(0x76, ms); return; }
            if (type == "F8") { keyPress(0x77, ms); return; }
            if (type == "F9") { keyPress(0x78, ms); return; }
            if (type == "F10") { keyPress(0x79, ms); return; }
            if (type == "F11") { keyPress(0x7A, ms); return; }
            if (type == "F12") { keyPress(0x7B, ms); return; }
            if (type == "F13") { keyPress(0x7C, ms); return; }
            if (type == "F14") { keyPress(0x7D, ms); return; }
            if (type == "F15") { keyPress(0x7E, ms); return; }
            if (type == "F16") { keyPress(0x7F, ms); return; }
            if (type == "F17") { keyPress(0x80, ms); return; }
            if (type == "F18") { keyPress(0x81, ms); return; }
            if (type == "F19") { keyPress(0x82, ms); return; }
            if (type == "F20") { keyPress(0x83, ms); return; }
            if (type == "F21") { keyPress(0x84, ms); return; }
            if (type == "F22") { keyPress(0x85, ms); return; }
            if (type == "F23") { keyPress(0x86, ms); return; }
            if (type == "F24") { keyPress(0x86, ms); return; }
        }
    }

    //Try to create binary VK code from integer 0-9
    try {
        int i = std::stoi(type);
        BYTE vk_code = VK_NUMPAD0 + i;
        keyPress(vk_code, ms);
    }
    catch (std::exception) {}
    //
}

extern "C" void __cdecl mouseTP(int x, int y) {
    SetCursorPos(x, y);
}

extern "C" void __cdecl mouseMove(int offsetX, int offsetY) {
    POINT pt;
    GetCursorPos(&pt);
    int steps = 1000;

    for (int i = 0; i < steps; i++) {
        int currX = pt.x + (offsetX * i) / steps;
        int currY = pt.y + (offsetY * i) / steps;
        mouse_event(MOUSEEVENTF_MOVE | MOUSEEVENTF_ABSOLUTE, currX, currY, 0, 0);
    }
}

extern "C" void __cdecl mouseKey(const char* btn_) {
    std::string btn = gsfcp(btn_);
    POINT pt;
    GetCursorPos(&pt);

    if (btn == "left_down") { mouse_event(MOUSEEVENTF_LEFTDOWN, pt.x, pt.y, 0, 0); return; }
    if (btn == "left_up") { mouse_event(MOUSEEVENTF_LEFTUP, pt.x, pt.y, 0, 0); return; }
    if (btn == "right_down") { mouse_event(MOUSEEVENTF_RIGHTDOWN, pt.x, pt.y, 0, 0); return; }
    if (btn == "right_up") { mouse_event(MOUSEEVENTF_RIGHTUP, pt.x, pt.y, 0, 0); return; }
    if (btn == "middle_down") { mouse_event(MOUSEEVENTF_MIDDLEDOWN, pt.x, pt.y, 0, 0); return; }
    if (btn == "middle_up") { mouse_event(MOUSEEVENTF_MIDDLEUP, pt.x, pt.y, 0, 0); return; }
}

extern "C" void __cdecl getCursorPos(int& x, int& y) {
    POINT pt;
    GetCursorPos(&pt);
    x = pt.x;
    y = pt.y;
}
