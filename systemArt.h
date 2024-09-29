#include <stdio.h>
#include <string.h>

char* getSystemArt(char* systemArt){
#ifdef _WIN32
    snprintf(systemArt, 256,"%s\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n",
            "llllllllll  llllllllll",
            "llllllllll  llllllllll",
            "llllllllll  llllllllll",
            "llllllllll  llllllllll",
            "",
            "llllllllll  llllllllll",
            "llllllllll  llllllllll",
            "llllllllll  llllllllll",
            "llllllllll  llllllllll");
    return systemArt;
#endif
}