#ifndef SECTION_WORK_H
#define SECTION_WORK_H

struct DATA;

BOOL AddSection(const char* filepath, const char* sectionName, DWORD sizeOfSection);
BOOL AddPE2Section(const char* filepath, DATA pe);
const char* GetRandomSectionName();


#endif 