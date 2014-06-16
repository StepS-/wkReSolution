
#pragma once

#include <ddraw.h>

typedef void(*PFVOID)();
typedef struct
{
	PVOID UnkTable1;
	DWORD Unk1, Unk2, Unk3, Unk4;
	PVOID UnkDD, UnkTable2;
	DWORD Unk5;
	DWORD RenderWidth, RenderHeight;
	DWORD Unk6, Unk7;
	DWORD WidthRT, HeightRT;
	DWORD HalfWidth, HalfHeight;
	DWORD Unk8;
	PCHAR UnkC;
	LPDIRECTDRAW lpDD;
} W2DDSTRUCT, *LPW2DDSTRUCT;

extern bool Cavern;
extern BYTE Version;
extern CHAR Config[MAX_PATH], LandFile[MAX_PATH];

extern HWND* pW2Wnd;
extern LPW2DDSTRUCT* pW2DS;
#define W2Wnd (*pW2Wnd)
#define W2DS (*pW2DS)
#define DDObj (W2DS->lpDD)

extern SHORT WinMinWidth;
extern SHORT SWidth, SHeight, GlobalEatLimit, TargetWidth, TargetHeight;
extern SHORT ScreenCX, ScreenCY, AeWidth, AeHeight;
extern BOOL OfflineCavernFloodFix;
extern BOOL AllowResize, ProgressiveResize, AltEnter;
extern BOOL AllowZoom, UseKeyboardZoom, UseMouseWheel;
extern PFVOID RenderGame;

BYTE CheckVersion();
BOOL CavernCheck();
BOOL GetW2WndSize(SHORT& sWidth, SHORT& sHeight);
void GetTargetScreenSize(SHORT nWidth, SHORT nHeight);
void GetAddresses();
void UnprotectAddresses();
void PatchMem(SHORT nWidth, SHORT nHeight, bool bMouseForWindow = false);
