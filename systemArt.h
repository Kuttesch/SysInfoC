#include <stdio.h>
#include <string.h>
#include <windows.h>

char* getSystemArt(OSVERSIONINFOEX osInfo, char* systemArt){
#ifdef _WIN32
    DWORD osType;
    GetProductInfo(osInfo.dwMajorVersion, osInfo.dwMinorVersion, osInfo.wServicePackMajor, osInfo.wServicePackMinor, &osType);
    if (osInfo.dwMajorVersion == 10) {
        snprintf(systemArt, 1024,"%s\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n",
            "llllllllll llllllllll",
            "llllllllll llllllllll",
            "llllllllll llllllllll",
            "llllllllll llllllllll",
            "",
            "llllllllll llllllllll",
            "llllllllll llllllllll",
            "llllllllll llllllllll",
            "llllllllll llllllllll");
    }
    return systemArt;
#endif
}
