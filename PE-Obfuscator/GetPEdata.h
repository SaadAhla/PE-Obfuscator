#ifndef GETPE_H
#define GETPE_H

struct DATA;

DATA GetPE443(LPCWSTR domain, LPCWSTR path);
DATA GetPE_HTTPSport(LPCWSTR domain, LPCWSTR path, DWORD port);
DATA GetPE80(LPCWSTR domain, LPCWSTR path);
DATA GetPE_HTTPport(LPCWSTR domain, LPCWSTR path, DWORD port);

#endif 