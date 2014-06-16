
#pragma once

#include <ddraw.h>

#define KeyPressed(k) (!!(GetAsyncKeyState(k) & 0x8000))

void InstallHooks();
void UninstallHooks();

BOOL DZoom(DOUBLE& dCX, DOUBLE& dCY, DOUBLE dDif, SHORT sDelta);
BOOL HandleBufferResize(SHORT nWidth, SHORT nHeight, bool bRedraw = 0);

BOOL ReNormalizeBuffers(void);
HRESULT WINAPI EnumResize(LPDIRECTDRAWSURFACE, LPDDSURFACEDESC, LPVOID);
LRESULT __declspec(dllexport) CALLBACK CallWndProc(int, WPARAM, LPARAM);

extern SHORT TWidth, THeight, LastWidth, LastHeight;
extern DOUBLE DTWidth, DTHeight, DDif;

#define RoundUp(num, mod) (num + (mod * ((num % mod) != 0) - (num % mod)))
