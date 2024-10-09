#include <stdio.h>
#include <string.h>
#include <windows.h>
#include "systemArt.h"

#ifdef __APPLE__
    #include <sys/sysctl.h>
    #include <mach/mach.h>
#endif

#define CHART_LENGTH 50
#define DIV 1024.0
#define GB_DIV (DIV * DIV)
#define TB_DIV (DIV * DIV * DIV)
#define COLOR 34

char* getCpuInfo();
int getMemInfo(float result[2]);
int getDiskInfo(char result[26][256], char drives[26]);
int getUpTime(int upTime[4]);
int getOsInfo(OSVERSIONINFOEX *osInfo, char osName[256]);
int matchWindowsVersion(OSVERSIONINFOEX osInfo, char osName[256]);
int infoToChart(float info[2], char outputChar[256]);
void print(const char *osArt, const char *systemInfo);


int main() {
    OSVERSIONINFOEX osInfo;
    char systemInfo[1024] = {0};
    char systemArt[1024] = {0};
    float memInfo[2];
    int upTime[4];
    char osName[256];
    char chartMem[256];
    char drives[26];
    char diskResult[26][256] = {0};
    char *cpuStr = getCpuInfo();

    getUpTime(upTime);
    getMemInfo(memInfo);
    infoToChart(memInfo, chartMem);
    getOsInfo(&osInfo, osName);
    matchWindowsVersion(osInfo, osName);
    getDiskInfo(diskResult,drives);
    getSystemArt(osInfo, systemArt);

    snprintf(systemInfo + strlen(systemInfo), 256, "OS:         %s\n", osName);
    snprintf(systemInfo + strlen(systemInfo), 256, "CPU:        %s\n", cpuStr);
    snprintf(systemInfo + strlen(systemInfo), 256, "Uptime:     %02d:%02d:%02d:%02d\n", upTime[0], upTime[1], upTime[2], upTime[3]);
    snprintf(systemInfo + strlen(systemInfo), 256, "Memory:     %s\n\n", chartMem);

    for (int i = 0; i < 26 && drives[i] != '\0'; i++) {
        snprintf(systemInfo + strlen(systemInfo), 256, "Disk %c:     %s\n", drives[i], diskResult[i]);
    }

    print(systemArt, systemInfo);
    return 0;
}

int a[10];
char* getCpuInfo() {
    static char cpuStr[256] = "";
    size_t size = sizeof(cpuStr);
    #ifdef _WIN32

        for (int i = 1; i <= 3; i++) {
            if (i == 1) {
                __asm__("mov $0x80000002, %eax");
            } else if (i == 2) {
                __asm__("mov $0x80000003, %eax");
            } else if (i == 3) {
                __asm__("mov $0x80000004, %eax");
            }
            __asm__("cpuid");
            __asm__("mov %%eax, %0" : "=r" (a[0]));
            __asm__("mov %%ebx, %0" : "=r" (a[1]));
            __asm__("mov %%ecx, %0" : "=r" (a[2]));
            __asm__("mov %%edx, %0" : "=r" (a[3]));
            strcat(cpuStr, (char*)&a[0]);
        }
    #elif __APPLE__
        #ifdef TARGET_OS_MAC
            char brandStr[size];
            if (sysctlbyname("machdep.cpu.brand_string", &brandStr, &size, NULL, 0) == 0) {
                strcat(cpuStr, brandStr);
            }
        #endif
    #endif
    return cpuStr;    
}

int getMemInfo(float result[2]) {
    #ifdef _WIN32
        MEMORYSTATUSEX memstat;
        memstat.dwLength = sizeof(memstat);
        GlobalMemoryStatusEx(&memstat);

        result[0] = (float)(memstat.ullTotalPhys - memstat.ullAvailPhys) / DIV;
        result[1] = (float)memstat.ullTotalPhys / DIV;
    #elif __linux__


    #elif __APPLE__
        #ifdef TARGET_OS_MAC
            int64_t totalMem;
            size_t size = sizeof(totalMem);
            mach_msg_type_number_t count = HOST_VM_INFO64_COUNT;
            vm_statistics64_data_t vm_info;
            kern_return_t kr = host_statistics64(mach_host_self(), HOST_VM_INFO64, (host_info64_t)&vm_info, &count);

            if (sysctlbyname("hw.memsize", &totalMem, &size, NULL, 0) == 0) {
                result[0] = (float)totalMem / DIV;
            } else {
                return 1;
            }
            if (kr == KERN_SUCCESS) {
                int64_t free_memory = vm_info.free_count * sysconf(_SC_PAGESIZE);
                int64_t active_memory = vm_info.active_count * sysconf(_SC_PAGESIZE);
                int64_t inactive_memory = vm_info.inactive_count * sysconf(_SC_PAGESIZE);
                int64_t wired_memory = vm_info.wire_count * sysconf(_SC_PAGESIZE);
                
                result[1] = (float)(active_memory + inactive_memory + wired_memory) / DIV;
            } else {
                return 1;
            }
        #endif
    #endif
    return 0;
}

int getDiskInfo(char result[26][256], char drives[26]) {
    ULARGE_INTEGER freeBytesAvailable, totalBytes, totalFreeBytes;
    char driveStrings[256];
    DWORD driveResult = GetLogicalDriveStringsA(sizeof(driveStrings), driveStrings);
    float info[2];
    char driveChar[256];
    char* drive = driveStrings;
    int i = 0;

    while (*drive && i < 26) {
        if (GetDiskFreeSpaceEx(drive, &freeBytesAvailable, &totalBytes, &totalFreeBytes)) {
            info[0] = (float)(totalBytes.QuadPart - freeBytesAvailable.QuadPart) / DIV;
            info[1] = (float)totalBytes.QuadPart / DIV;

            infoToChart(info, driveChar);
            strncpy(result[i], driveChar, 256);
            drives[i] = drive[0];
            i++;
        }
        drive += strlen(drive) + 1;
    }
    drives[i] = '\0';

    return 0;
}

int getUpTime(int upTime[4]) {
    ULONGLONG tickCount = GetTickCount64();
    int sec = (int)(tickCount / 1000);

    upTime[0] = sec / 86400;
    sec %= 86400;
    upTime[1] = sec / 3600;
    sec %= 3600;
    upTime[2] = sec / 60;
    upTime[3] = sec % 60;

    return 0;
}

int getOsInfo(OSVERSIONINFOEX *osInfo, char osName[256]) {
    ZeroMemory(osInfo, sizeof(OSVERSIONINFOEX));
    osInfo->dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
    GetVersionEx((LPOSVERSIONINFO)osInfo);
    matchWindowsVersion(*osInfo, osName);
    return 0;
}

int matchWindowsVersion(OSVERSIONINFOEX osInfo, char osName[256]) {
    DWORD osType;
    GetProductInfo(osInfo.dwMajorVersion, osInfo.dwMinorVersion, osInfo.wServicePackMajor, osInfo.wServicePackMinor, &osType);

    if (osInfo.dwMajorVersion == 10 && osInfo.dwBuildNumber >= 22000) {
        strcpy(osName, "Windows 11");
    } else if (osInfo.dwMajorVersion == 10) {
        strcpy(osName, "Windows 10");
    } else {
        strcpy(osName, "Older Windows");
        return 1;
    }

    switch (osType) {
        case PRODUCT_HOME_BASIC: strcat(osName, " Home Basic"); break;
        case PRODUCT_HOME_PREMIUM: strcat(osName, " Home Premium"); break;
        case PRODUCT_PROFESSIONAL: strcat(osName, " Professional"); break;
        case PRODUCT_ULTIMATE: strcat(osName, " Ultimate"); break;
        case PRODUCT_ENTERPRISE: strcat(osName, " Enterprise"); break;
        case PRODUCT_EDUCATION: strcat(osName, " Education"); break;
        case PRODUCT_CORE: strcat(osName, " Home"); break;
        case PRODUCT_PRO_WORKSTATION: strcat(osName, " Pro for Workstations"); break;
        default: strcat(osName, ""); break;
    }
    return 0;
}


int infoToChart(float info[2], char outputChar[256]) {

    char occupBuffer[CHART_LENGTH + 1] = {0};
    char freeBuffer[CHART_LENGTH + 1] = {0};
    char frontBuffer[CHART_LENGTH / 2 + 1] = {0};
    char midBuffer[256] = {0};
    char backBuffer[CHART_LENGTH / 2 + 1] = {0};

    int occupPerc = (info[0] / info[1]) * 100;
    int freePerc = 100 - occupPerc;

    char occupChar[50] = {0};
    char totalChar[50] = {0};

    if (info[0] > TB_DIV) {
        snprintf(occupChar, sizeof(occupChar), "  %6.2f TB", info[0] / TB_DIV);
    } else if (info[0] > GB_DIV) {
        snprintf(occupChar, sizeof(occupChar), "  %6.2f GB", info[0] / GB_DIV);
    } else {
        snprintf(occupChar, sizeof(occupChar), "  %6.0f MB", info[0]);
    }

    if (info[1] > TB_DIV) {
        snprintf(totalChar, sizeof(totalChar), "%6.2f TB  ", info[1] / TB_DIV);
    } else if (info[1] > GB_DIV) {
        snprintf(totalChar, sizeof(totalChar), "%6.2f GB  ", info[1] / GB_DIV);
    } else {
        snprintf(totalChar, sizeof(totalChar), "%6.0f MB  ", info[1]);
    }

    snprintf(midBuffer, 256, "%s/%s", occupChar, totalChar);
    int midBufferLength = strlen(midBuffer) / 2;

    int occupBarLen = (occupPerc * CHART_LENGTH) / 100;
    int freeBarLen = CHART_LENGTH - occupBarLen;

    memset(occupBuffer, '/', occupBarLen);
    memset(freeBuffer, ' ', freeBarLen);

    char chartBuffer[CHART_LENGTH + 1] = {0};
    snprintf(chartBuffer, sizeof(chartBuffer), "%s%s", occupBuffer, freeBuffer);

    strncpy(frontBuffer, chartBuffer, (CHART_LENGTH / 2) - midBufferLength);
    strncpy(backBuffer, chartBuffer + (CHART_LENGTH / 2) + midBufferLength, CHART_LENGTH / 2);

    snprintf(outputChar, 256, "[%s%s%s] %d%%", frontBuffer, midBuffer, backBuffer, occupPerc);

    return 0;
}

void print(const char *osArt, const char *systemInfo) {
    const char *line1 = osArt;
    const char *line2 = systemInfo;

    while (*line1 != '\0' || *line2 != '\0') {
        const char *nextLine1 = strchr(line1, '\n');
        const char *nextLine2 = strchr(line2, '\n');
        
        char buffer1[60] = {0};
        char buffer2[60] = {0};
        
        if (nextLine1 != NULL) {
            strncpy(buffer1, line1, nextLine1 - line1);
            line1 = nextLine1 + 1;
        } else {
            strcpy(buffer1, line1);
            line1 += strlen(line1);
        }

        if (nextLine2 != NULL) {
            strncpy(buffer2, line2, nextLine2 - line2);
            line2 = nextLine2 + 1;
        } else {
            strcpy(buffer2, line2);
            line2 += strlen(line2);
        }
        char color[11];
        char resetColor[11];
        snprintf(color, 11, "\033[0;%dm", COLOR);
        snprintf(resetColor, 11, "\033[0m");

        printf("%s %-30s %s %-30s\n",color, buffer1, resetColor, buffer2);
    }
}
