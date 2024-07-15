#include <windows.h>
#include <iostream>
#include <fstream>

//Compiler flags: -s -static-libstdc++ -static-libgcc -static  -luser32

std::string gsfcp(const char* s) { return std::string(s); }

extern "C" bool __cdecl access(const char* filePath) {
    std::ifstream file(gsfcp(filePath));
    if (!file.is_open()) {
        return false;
    }
    file.close();
    return true;
}
