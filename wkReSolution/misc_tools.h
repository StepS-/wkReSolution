
#pragma once

#define ImageBase ((DWORD)GetModuleHandleA(0))
#define MemOffset(offset) (ImageBase + offset)
#define ModuleImageBase(module) ((DWORD)GetModuleHandleA(module))
#define ModuleMemOffset(module, offset) (ModuleImageBase(module) + offset)

DWORD GetPETimestampA(LPCSTR lpModuleName);
BOOL WritePrivateProfileIntA(LPCSTR lpAppName, LPCSTR lpKeyName, int nInteger, LPCSTR lpFileName);
LPSTR GetPathUnderExeA(LPSTR OutBuf, LPCSTR FileName);
BOOL Unprotect(ULONG addr, SIZE_T dwSize = sizeof(WORD));
