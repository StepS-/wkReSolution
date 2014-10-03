
#pragma once

#include <ddraw.h>

#define WWP (Version < 0)
#define W2 (Version > 0)

#define W2_15 1
#define WWP_10 -1
#define WWP_11 -2

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

typedef struct
{
	DWORD X, Y;
} LCOORD, *LPLCOORD;

extern bool Cavern;
extern char Version;
extern CHAR Config[MAX_PATH], LandFile[MAX_PATH], GameFile[MAX_PATH];

extern HWND* pWormsWnd;
extern LPW2DDSTRUCT* pW2DS;
extern LPDIRECTDRAW2* wwpDD;
extern HWND* pT17Wnd;
extern PVOID* WWPCurPosStruct;
extern BOOL* pWWPInGame;
#define W2DS (*pW2DS)
#define T17Wnd (*pT17Wnd)

extern DWORD WinMinWidth;
extern DWORD SWidth, SHeight, GlobalEatLimit, TargetWidth, TargetHeight;
extern DWORD ScreenCX, ScreenCY, AeWidth, AeHeight;
extern BOOL OfflineCavernFloodFix;
extern BOOL AllowResize, ProgressiveResize, AltEnter;
extern BOOL AllowZoom, UseKeyboardZoom, UseMouseWheel, UseTouchscreenZoom;
extern PFVOID RenderGame;

LPDIRECTDRAW DDObj();
BOOL InGame();
HWND WormsWnd();
HWND InputWnd();
LPLCOORD GCursPos();
char CheckVersion();
void CavernCheck();
BOOL GetWndSize(HWND hWnd, DWORD& sWidth, DWORD& sHeight);
void GetTargetScreenSize(DWORD nWidth, DWORD nHeight);
void PrepareAddresses();
void PatchW2Mem(DWORD nWidth, DWORD nHeight, bool bMouseForWindow = false);

BOOL UpdateCenteredCursor(DWORD nWidth, DWORD nHeight, bool bMouseForWindow = false);
void SetWWPRenderingDimensions(DWORD nWidth, DWORD nHeight, bool bMouseForWindow = false);
void ProcessW2Waterrise();
void ProcessWWPWater();
void ProcessWWPWaterInit();
void ProcessWWPWaterInitCont();
void ProcessWWPWaterRise();
void ProcessDDStartup();
