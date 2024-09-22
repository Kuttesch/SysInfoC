#include<stdio.h>
#include<string.h>

int a[10];
char* getCpuID()
{
    __asm__(
        "xor %eax , %eax\n\t"
        "xor %ebx , %ebx\n\t"
        "xor %ecx , %ecx\n\t"
        "xor %edx , %edx\n\t"
    );

    static char cpu_str[256] = "";

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
        strcat(cpu_str, (char*)&a[0]);
    }
    
    return cpu_str;
}

int main(){
    char *cpuStrPtr = getCpuID();


    printf("%s", cpuStrPtr);
}