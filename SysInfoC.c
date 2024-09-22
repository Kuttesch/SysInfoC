#include<stdio.h>
#include<string.h>
#include<windows.h>

#define DIV 1024

int a[10];

char* getCpuInfo();
int getMemInfoWindows();

int main(){
    float memInfo[3];
    char *cpuStr = getCpuInfo();
    getMemInfoWindows(memInfo);

    printf("%s\n", cpuStr);
    printf("MEMPerct:   %f %%\n",memInfo[0]);
    printf("MEMTotal:   %f GB\n",memInfo[1]);
    printf("MEMAvail:   %f MB\n",memInfo[2]);
}

char* getCpuInfo(){
    __asm__(
        "xor %eax , %eax\n\t"
        "xor %ebx , %ebx\n\t"
        "xor %ecx , %ecx\n\t"
        "xor %edx , %edx\n\t"
    );

    static char cpuStr[256] = "";

    for (int i=1; i<=3; i++){
        if (i == 1) {
            __asm__("mov $0x80000002, %eax");
        }
        else if (i == 2) {
            __asm__("mov $0x80000003, %eax");
        }
        else if (i == 3) {
            __asm__("mov $0x80000004, %eax");
        }
        __asm__("cpuid");
        __asm__("mov %%eax, %0" : "=r" (a[0]));
        __asm__("mov %%ebx, %0" : "=r" (a[1]));
        __asm__("mov %%ecx, %0" : "=r" (a[2]));
        __asm__("mov %%edx, %0" : "=r" (a[3]));
        strcat(cpuStr, (char*)&a[0]);
    }
    return cpuStr;    
}

int getMemInfoWindows(float result[3]){
    
    MEMORYSTATUSEX memstat;
    memstat.dwLength = sizeof(memstat);
    GlobalMemoryStatusEx (&memstat);

    result[0] = (float)memstat.dwMemoryLoad;
    result[1] = (float)memstat.ullTotalPhys / (DIV * DIV * DIV);
    result[2] = (float)memstat.ullAvailPhys / (DIV * DIV);

    return 0;
}
