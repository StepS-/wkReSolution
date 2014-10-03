
#pragma once

typedef unsigned long long QWORD;
typedef QWORD *PQWORD, *LPQWORD;

extern "C" IMAGE_DOS_HEADER __ImageBase;
#define HINST_THISCOMPONENT ((HINSTANCE)&__ImageBase)

#define IJ_JUMP 0 //Insert a jump (0xE9) with InsertJump
#define IJ_CALL 1 //Insert a call (0xE8) with InsertJump
#define IJ_FARJUMP 2 //Insert a farjump (0xEA) with InsertJump
#define IJ_FARCALL 3 //Insert a farcall (0x9A) with InsertJump
#define IJ_PUSHRET 4 //Insert a pushret with InsertJump

BOOL WritePrivateProfileIntA(LPCSTR lpAppName, LPCSTR lpKeyName, int nInteger, LPCSTR lpFileName);
LPSTR GetPathUnderModuleA(HMODULE hModule, LPSTR OutBuf, LPCSTR FileName);

BOOL __stdcall InsertJump(PVOID pDest, DWORD dwPatchSize, PVOID pCallee, DWORD dwJumpType = IJ_JUMP);

BOOL __stdcall PatchMemData(ULONG, PVOID, ULONG);
BOOL __stdcall PatchMemQword(ULONG, QWORD);
BOOL __stdcall PatchMemDword(ULONG, DWORD);
BOOL __stdcall PatchMemWord(ULONG, WORD);
BOOL __stdcall PatchMemByte(ULONG, BYTE);
BOOL __stdcall PatchMemFloat(ULONG, FLOAT);
BOOL __stdcall PatchMemDouble(ULONG, DOUBLE);
BOOL __stdcall PatchMemStringA(ULONG, ULONG, LPSTR);
BOOL __stdcall PatchMemStringW(ULONG, ULONG, LPWSTR);

typedef struct PEInfo
{
	PEInfo(HMODULE hModule = 0);
	~PEInfo() {};

	void Reset(HMODULE hModule);
	DWORD Offset(DWORD off);
	BOOL PtrInCode(PVOID ptr);
	BOOL PtrInData(PVOID ptr);

	HANDLE Handle;
	IMAGE_DOS_HEADER* DOS;
	IMAGE_NT_HEADERS* NT;
	IMAGE_FILE_HEADER* FH;
	IMAGE_OPTIONAL_HEADER* OPT;
} *PPEInfo;
