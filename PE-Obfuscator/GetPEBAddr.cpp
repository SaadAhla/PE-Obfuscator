#include "GetPEBAddr.h"


char* XOR (char* string) {
	for (int i = 0; i < strlen(string); i++) {
		string[i] ^= 0xaa;
	}
	return string;
}


DWORD calcHash(PCSTR data) {
	DWORD hash = 0x99;
	for (int i = 0; i < strlen(data); i++) {
		hash += data[i] + (hash << 1);
	}
	return hash;
}


HMODULE PEBGetAddr(DWORD64 FunctionHash)
{
	DWORD FuncNumber;
	WORD OrdinalIndex;
	PDWORD FuncNameBase;
	PCSTR FunctionName;
	PIMAGE_EXPORT_DIRECTORY ExportDir;
	DWORD i;


	// get a pointer to the PEB structure located at GS:[0x60]
	PPEB PebAddress = (PPEB)__readgsqword(0x60);

	// get a pointer to the PEB_LDR_DATA structure located at offset 0x18 in the PEB
	PNEW_PEB_LDR_DATA Ldr = (PNEW_PEB_LDR_DATA)PebAddress->Ldr;

	// get a pointer to the first Flink Entry "holding information about the 1st module : injectedProcess.exe" 
	// of the InLoadOrderModuleList nested struct in PEB_LDR_DATA struct.
	PLIST_ENTRY NextModule = Ldr->InLoadOrderModuleList.Flink;


	PNEW_LDR_DATA_TABLE_ENTRY DataTableEntry = (PNEW_LDR_DATA_TABLE_ENTRY)NextModule;
	// we iterate over the linkedlist entries , the last entry is null, 
	// that's why we check if the last Module->DllBase is null to stop
	while (DataTableEntry->DllBase != NULL)
	{
		// Getting the baseaddress of the current module "Entry"
		DWORD64 ModuleBase = (DWORD64)DataTableEntry->DllBase;

		// Getting a pointer to the NTHEADER structer of the current module
		PIMAGE_NT_HEADERS NTHeader = (PIMAGE_NT_HEADERS)(ModuleBase + ((PIMAGE_DOS_HEADER)ModuleBase)->e_lfanew);

		// Get the Export Directory RVA for the current module
		DWORD ExportDirRVA = NTHeader->OptionalHeader.DataDirectory[0].VirtualAddress;

		// Get the next loaded module entry , which is ntdll.dll, kernel32.dll, ...
		DataTableEntry = (PNEW_LDR_DATA_TABLE_ENTRY)DataTableEntry->InLoadOrderLinks.Flink;

		// If the current module doesn't have an export directory , move on to the next module.
		if (ExportDirRVA == 0)
		{
			continue;
		}

		// gets into the Export Directory Table
		ExportDir = (PIMAGE_EXPORT_DIRECTORY)((DWORD64)ModuleBase + ExportDirRVA);
		// total number of exported functions.
		FuncNumber = ExportDir->NumberOfNames;
		// list of exported symbol name's RVA
		FuncNameBase = (PDWORD)((PCHAR)ModuleBase + ExportDir->AddressOfNames);

		for (i = 0; i < FuncNumber; i++)
		{
			// the current Function name 
			FunctionName = (PCSTR)(*FuncNameBase + (DWORD64)ModuleBase);
			// next function name
			FuncNameBase++;


			// if it's equal to the submitted hash to the getAPIAddr() function
			if (calcHash(FunctionName) == FunctionHash)
			{
				// pointer to the ordinal of the submitted function	
				OrdinalIndex = *(PWORD)(((DWORD64)ModuleBase + ExportDir->AddressOfNameOrdinals) + (2 * i));
				// pointer to the base address of the function : baseaddress of the module +  the RVA of that function =>
				// base address of the module + value pointed by ( Module base address + RVA of AddressOfFunctions + the ordinal corresponding to that function * 4 Bytes "4 bytes for each RVA").
				return (HMODULE)((DWORD64)ModuleBase + *(PDWORD)(((DWORD64)ModuleBase + ExportDir->AddressOfFunctions) + (4 * OrdinalIndex)));

			}
		}
	}

	return NULL;
}