
#pragma once

#include <ddraw.h>

void EndMadHook();
void UpdateW2DDSizeStruct();
void W2DDInitHook();
void InstallHooks();
void UninstallHooks();

HRESULT WINAPI EnumResize(LPDIRECTDRAWSURFACE, LPDDSURFACEDESC, LPVOID);
LRESULT __declspec(dllexport) CALLBACK CallWndProc(int, WPARAM, LPARAM);
HRESULT WINAPI DirectDrawCreateHook(GUID*, LPDIRECTDRAW*, IUnknown*);
HWND WINAPI CreateWindowExAHook(DWORD, LPCSTR, LPCSTR, DWORD, int, int, int, int, HWND, HMENU, HINSTANCE, LPVOID);

extern HRESULT(WINAPI *DirectDrawCreateNext)(GUID*, LPDIRECTDRAW*, IUnknown*);
extern HWND(WINAPI *CreateWindowExANext)(DWORD, LPCSTR, LPCSTR, DWORD, int, int, int, int, HWND, HMENU, HINSTANCE, LPVOID);
extern SHORT TWidth, THeight, LastWidth, LastHeight;
extern PVOID W2DDHookStart, W2DDHookNext;

#define CVal(num, val) (!!(num & val))
#define RoundUp(num, mod) (num + (mod * ((num % mod) != 0) - (num % mod)))
