#include <Windows.h>
#include <stdio.h>
#include "DropFile.h"

struct DATA {
    LPVOID data;
    DWORD size;
};


VOID DropIt(DATA data, const char* name) {

    char Dir[MAX_PATH];
    if (!GetCurrentDirectoryA(MAX_PATH, Dir)) {
        printf("Error GetCurrentDirectory !!");
        return;
    }

    char filepath[MAX_PATH];
    ZeroMemory(filepath, MAX_PATH);
    lstrcatA(filepath, Dir);
    lstrcatA(filepath, "\\");
    lstrcatA(filepath, name);
    printf("filepath : %s\n", filepath);
    
    HANDLE file = CreateFileA(filepath, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (file == INVALID_HANDLE_VALUE) {
        printf("INVALID FILE HANDLE\n");
        return;
    }

    DWORD dw;

    WriteFile(file, data.data, data.size, &dw, 0);
    

}