
#include <Windows.h>
#include <stdio.h>
#include "misc_tools.h"

BOOL WritePrivateProfileIntA(LPCSTR lpAppName, LPCSTR lpKeyName, int nInteger, LPCSTR lpFileName)
{
	CHAR lpString[32];
	sprintf_s(lpString, "%d", nInteger);
	return WritePrivateProfileStringA(lpAppName, lpKeyName, lpString, lpFileName);
}

LPSTR GetPathUnderModuleA(HMODULE hModule, LPSTR OutBuf, LPCSTR FileName)
{
	GetModuleFileNameA(hModule, OutBuf, MAX_PATH);
	CHAR* dirend = strrchr(OutBuf, '\\') + 1;
	strcpy_s(dirend, MAX_PATH, FileName);
	return OutBuf;
}

PEInfo::PEInfo(HMODULE hModule)
{
	Reset(hModule);
}

void PEInfo::Reset(HMODULE hModule)
{
	hModule = hModule == 0 ? GetModuleHandleA(0) : hModule;
	Handle = hModule;
	DOS = (IMAGE_DOS_HEADER*)hModule;
	NT = (IMAGE_NT_HEADERS*)((DWORD)DOS + DOS->e_lfanew);
	FH = (IMAGE_FILE_HEADER*)&NT->FileHeader;
	OPT = (IMAGE_OPTIONAL_HEADER*)&NT->OptionalHeader;
}

DWORD PEInfo::Offset(DWORD off)
{
	return (DWORD)Handle + off;
}

BOOL PEInfo::PtrInCode(PVOID ptr)
{
	if (DWORD(ptr) >= Offset(OPT->BaseOfCode) &&
		DWORD(ptr) < Offset(OPT->BaseOfCode) + OPT->SizeOfCode)
		return true;
	return false;
}

BOOL PEInfo::PtrInData(PVOID ptr)
{
	if (DWORD(ptr) >= Offset(OPT->BaseOfData) &&
		DWORD(ptr) < Offset(OPT->BaseOfData) + OPT->SizeOfInitializedData + OPT->SizeOfUninitializedData)
		return true;
	return false;
}

BOOL __stdcall PatchMemData(ULONG dwAddr, ULONG dwBufLen, PVOID pNewData, ULONG dwDataLen)
{
	if (!dwDataLen || !pNewData || !dwAddr)
	{
		SetLastError(ERROR_INVALID_PARAMETER);
		return 0;
	}
	DWORD dwLastProtection;
	if (!VirtualProtect((void*)dwAddr, dwDataLen, PAGE_EXECUTE_READWRITE, &dwLastProtection))
		return 0;

	memcpy_s((PVOID)dwAddr, dwBufLen, pNewData, dwDataLen);

	return VirtualProtect((void*)dwAddr, dwDataLen, dwLastProtection, &dwLastProtection);
}

BOOL __stdcall PatchMemQword(ULONG dwAddr, QWORD qNewValue) { return PatchMemData(dwAddr, 8, &qNewValue, 8); }
BOOL __stdcall PatchMemDword(ULONG dwAddr, DWORD dwNewValue) { return PatchMemData(dwAddr, 4, &dwNewValue, 4); }
BOOL __stdcall PatchMemWord(ULONG dwAddr, WORD wNewValue) { return PatchMemData(dwAddr, 2, &wNewValue, 2); }
BOOL __stdcall PatchMemByte(ULONG dwAddr, BYTE bNewValue) { return PatchMemData(dwAddr, 1, &bNewValue, 1); }
BOOL __stdcall PatchMemStringA(ULONG dwAddr, ULONG dwDestLen, LPSTR lpString) { return PatchMemData(dwAddr, dwDestLen, lpString, strlen(lpString) + 1); }
BOOL __stdcall PatchMemStringW(ULONG dwAddr, ULONG dwDestLen, LPWSTR lpString) { return PatchMemData(dwAddr, dwDestLen*2, lpString, wcslen(lpString) + 2); }

BOOL __stdcall PatchMemFloat(ULONG dwAddr, FLOAT fNewValue)
{
	DWORD dwLastProtection;
	if (!VirtualProtect((void*)dwAddr, sizeof(FLOAT), PAGE_EXECUTE_READWRITE, &dwLastProtection))
		return 0;
	*(FLOAT*)dwAddr = fNewValue;
	return VirtualProtect((void*)dwAddr, sizeof(FLOAT), dwLastProtection, &dwLastProtection);
}

BOOL __stdcall PatchMemDouble(ULONG dwAddr, DOUBLE dNewValue)
{
	DWORD dwLastProtection;
	if (!VirtualProtect((void*)dwAddr, sizeof(DOUBLE), PAGE_EXECUTE_READWRITE, &dwLastProtection))
		return 0;
	*(DOUBLE*)dwAddr = dNewValue;
	return VirtualProtect((void*)dwAddr, sizeof(DOUBLE), dwLastProtection, &dwLastProtection);
}

BOOL __stdcall InsertJump(PVOID pDest, DWORD dwPatchSize, PVOID pCallee, DWORD dwJumpType)
{
	if (dwPatchSize >= 5 && pDest)
	{
		DWORD OpSize = 5, OpCode = 0xE9;
		switch (dwJumpType)
		{
		case IJ_PUSHRET:
			OpSize = 6;
			OpCode = 0x68;
			break;
		case IJ_FARJUMP:
			OpSize = 7;
			OpCode = 0xEA;
			break;
		case IJ_FARCALL:
			OpSize = 7;
			OpCode = 0x9A;
			break;
		case IJ_CALL:
			OpSize = 5;
			OpCode = 0xE8;
			break;
		default:
			OpSize = 5;
			OpCode = 0xE9;
			break;
		}

		if (dwPatchSize < OpSize)
			return 0;

		PatchMemByte((ULONG)pDest, (BYTE)OpCode);

		switch (OpSize)
		{
		case 7:
			PatchMemDword((ULONG)pDest + 1, (ULONG)pCallee);
			PatchMemWord((ULONG)pDest + 5, 0x23);
			break;
		case 6:
			PatchMemDword((ULONG)pDest + 1, (ULONG)pCallee);
			PatchMemByte((ULONG)pDest + 5, 0xC3);
			break;
		default:
			PatchMemDword((ULONG)pDest + 1, (ULONG)pCallee - (ULONG)pDest - 5);
			break;
		}

		for (ULONG i = OpSize; i < dwPatchSize; i++)
			PatchMemByte((ULONG)pDest + i, 0x90);
	}
	return 0;
}
