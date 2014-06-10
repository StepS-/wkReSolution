
#pragma once

typedef void(*PFVOID)();

extern bool Cavern;
extern BYTE Version;
extern CHAR Config[MAX_PATH], LandFile[MAX_PATH];

extern HWND* pW2Wnd;
#define W2Wnd (*pW2Wnd)

extern SHORT WinMinWidth;
extern SHORT SWidth, SHeight, GlobalEatLimit, TargetWidth, TargetHeight;
extern SHORT ScreenCX, ScreenCY, AeWidth, AeHeight;
extern BOOL OfflineCavernFloodFix;
extern BOOL AllowResize, ProgressiveResize, AltEnter;
extern BOOL AllowZoom, UseKeyboardZoom, UseMouseWheel;
extern PFVOID RenderGame;
extern PVOID W2DDInitStart, W2DDInitNext;
extern PVOID W2DDCreateStart, W2DDCreateNext;

BYTE CheckVersion();
BOOL CavernCheck();
BOOL GetW2WndSize(SHORT& sWidth, SHORT& sHeight);
void GetTargetScreenSize(SHORT nWidth, SHORT nHeight);
void GetAddresses();
void UnprotectAddresses();
void PatchMem(SHORT nWidth, SHORT nHeight, bool bMouseForWindow = false);
