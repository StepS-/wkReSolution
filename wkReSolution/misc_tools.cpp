
#include <Windows.h>
#include <stdio.h>
#include "misc_tools.h"

DWORD GetPETimestampA(LPCSTR lpModuleName)
{
	DWORD PEOffset = *(PDWORD)(ImageBase + 0x3C);
	return *(PDWORD)(ImageBase + PEOffset + 0x08);
}

BOOL WritePrivateProfileIntA(LPCSTR lpAppName, LPCSTR lpKeyName, int nInteger, LPCSTR lpFileName)
{
	CHAR lpString[32];
	sprintf_s(lpString, "%d", nInteger);
	return WritePrivateProfileStringA(lpAppName, lpKeyName, lpString, lpFileName);
}

LPSTR GetPathUnderExeA(LPSTR OutBuf, LPCSTR FileName)
{
	GetModuleFileNameA(NULL, OutBuf, MAX_PATH);
	CHAR* dirend = strrchr(OutBuf, '\\') + 1;
	strcpy_s(dirend, MAX_PATH, FileName);
	return OutBuf;
}

BOOL Unprotect(ULONG addr, SIZE_T dwSize)
{
	DWORD uselessDword;
	return VirtualProtect((void*)addr, dwSize, PAGE_READWRITE, &uselessDword);
}
