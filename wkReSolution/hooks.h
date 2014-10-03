
#pragma once

#include <ddraw.h>

#define KeyPressed(k) (!!(GetAsyncKeyState(k) & 0x8000))

void InstallHooks();
void UninstallHooks();

BOOL DZoom(DOUBLE& dCX, DOUBLE& dCY, DOUBLE dDif, SHORT sDelta);
BOOL HandleBufferResize(DWORD nWidth, DWORD nHeight, bool bRedraw = 0);

BOOL ReNormalizeBuffers(void);
HRESULT WINAPI EnumResize(LPDIRECTDRAWSURFACE, LPDDSURFACEDESC, LPVOID);
HRESULT WINAPI EnumCleanup(LPDIRECTDRAWSURFACE, LPDDSURFACEDESC, LPVOID);
BOOL CleanupSurfaces();
LRESULT CALLBACK CallWndProc(int, WPARAM, LPARAM);

extern DWORD TWidth, THeight, LastWidth, LastHeight;
extern DOUBLE DTWidth, DTHeight, DDif;

#define RoundUp(num, mod) (num + (mod * ((num % mod) != 0) - (num % mod)))
