
#include <windows.h>
#include <stdio.h>
#include "hooks.h"
#include "w2res.h"
#include "misc_tools.h"

bool Cavern;
char Version;
PEInfo EXE;
CHAR Config[MAX_PATH], LandFile[MAX_PATH], GameFile[MAX_PATH];

HWND* pWormsWnd;
LPW2DDSTRUCT* pW2DS;
LPLCOORD W2PosCoord;

DWORD WinMinWidth;
DWORD SWidth, SHeight, TargetWidth, TargetHeight;
DWORD ScreenCX, ScreenCY, AeWidth, AeHeight;
DWORD GlobalEatLimit;

BOOL AllowResize, ProgressiveResize, AltEnter;
BOOL AllowZoom, UseKeyboardZoom, UseMouseWheel, UseTouchscreenZoom;
DWORD ActualWidth, HorizontalSidesBox, RenderFromLeft;
DWORD LandWaterCriticalZone, CavernWaterEatLimit, ActualHeight, HUnk4, HUnk5, RenderFromTop, VerticalSidesBox;
DWORD LeftOffset;
DWORD AL_WUnk2, AL_HorizontalSidesBox, AL_RenderFromLeft, AL_RenderFromTop, AL_TopInfidelBox, AL_SWUnk1, AL_LeftOffset;
DWORD TopOffset;

DWORD WidthNow, HeightNow;
DWORD WWPDDinit, WWPDDterm;
DWORD WWPGameEndCont;
LPDIRECTDRAW2* wwpDD;
HWND* pT17Wnd;
BOOL* pWWPInGame;
PVOID* WWPCurPosStruct;

DWORD WateredScreenHeight;
DWORD MapWidth = 1920, MapHeight = 696;

PFVOID RenderGame;

char CheckVersion()
{
	DWORD PETime = EXE.FH->TimeDateStamp;
	if (PETime >= 0x352118A5 && PETime <= 0x352118FD)
		return Version = W2_15;
	else if (PETime >= 0x3AFFFAAB && PETime <= 0x3AFFFBB1)
		return Version = WWP_11;
	else if (PETime == 0x3A92A062 && PETime <= 0x3A92A27E)
		return Version = WWP_10;
	return Version = 0;
}

void CavernCheck()
{
	FILE *fLand, *fGame;
	int cavc, borc;
	Cavern = false;
	if (fopen_s(&fLand, GetPathUnderModuleA(NULL, LandFile, "Data\\land.dat"), "r") == ERROR_SUCCESS)
	{
		fseek(fLand, 0x08, SEEK_SET);
		fread_s(&MapWidth, 4, 4, 1, fLand);
		fread_s(&MapHeight, 4, 4, 1, fLand);
		if ((cavc = fgetc(fLand)) != 0)
			Cavern = true;
		else if (!WWP)
		{
			if (fopen_s(&fGame, GetPathUnderModuleA(NULL, GameFile, "Data\\game.dat"), "r") == ERROR_SUCCESS)
			{
				fseek(fGame, 0xC4F, SEEK_SET);
				if ((borc = fgetc(fGame)) != 0)
					Cavern = true;
				fclose(fGame);
			}
			else
				MessageBoxA(NULL,
				"Warning: failed to open the \"Data\\game.dat\" file. "
				"Something is bad.", "ReSolution warning",
				MB_OK | MB_ICONWARNING);
		}
		else
		{
			if (fopen_s(&fGame, GetPathUnderModuleA(NULL, GameFile, "Data\\current.thm"), "r") == ERROR_SUCCESS)
			{
				DWORD dwMapVer;
				fread_s(&dwMapVer, 4, 4, 1, fGame);
				if (dwMapVer < 3)
				{
					fseek(fGame, 0x14, SEEK_SET);
					if ((borc = fgetc(fGame)) == 0)
						Cavern = true;
					fclose(fGame);
				}
			}
			else if (fopen_s(&fGame, GetPathUnderModuleA(NULL, GameFile, "custom.dat"), "r") == ERROR_SUCCESS)
			{

				fseek(fGame, 0x10, SEEK_SET);
				if ((borc = fgetc(fGame)) == 0)
					Cavern = true;
				fclose(fGame);
			}
		}
		fclose(fLand);
	}
	else
		MessageBoxA(NULL,
		"Warning: failed to open the \"Data\\land.dat\" file. "
		"Your game will most likely crash.", "ReSolution warning",
		MB_OK | MB_ICONWARNING);

	if (!WateredScreenHeight)
	if (Cavern)
		WateredScreenHeight = MapHeight + 158;
	else
		WateredScreenHeight = 2048 + MapHeight + 158;
}

BOOL InGame()
{
	if (WWP)
		return *pWWPInGame;
	else
		return TRUE;
}

LPDIRECTDRAW DDObj()
{
	if (WWP)
		return *(LPDIRECTDRAW*)wwpDD;
	else
		return W2DS->lpDD;
}

HWND WormsWnd()
{
	if (WWP)
	{
		if (*pWormsWnd)
			return *(HWND*)((DWORD)(*pWormsWnd) + 0x20);
		else
			return 0;
	}
	else
		return *pWormsWnd;
}

HWND InputWnd()
{
	if (WWP)
		return *pT17Wnd;
	else
		return *pWormsWnd;
}

LPLCOORD GCursPos()
{
	if (WWP)
	{
		if (*WWPCurPosStruct)
			return (LPLCOORD)((DWORD)(*WWPCurPosStruct) + 0x3C);
		else
			return 0;
	}
	else
		return W2PosCoord;
}

BOOL GetWndSize(HWND hWnd, DWORD& sWidth, DWORD& sHeight)
{
	BOOL result = 0;
	if (IsWindow(hWnd))
	{
		RECT WRect;
		if (GetClientRect(hWnd, &WRect))
		{
			sWidth = WRect.right - WRect.left;
			sHeight = WRect.bottom - WRect.top;
			result = 1;
		}
	}
	return result;
}

void GetTargetScreenSize(DWORD nWidth, DWORD nHeight)
{
	if (Cavern)
	{
		TargetWidth = nWidth > MapWidth - 4 ? MapWidth - 4 : nWidth;
		TargetHeight = nHeight > WateredScreenHeight ? WateredScreenHeight : nHeight;
	}
	else
	{
		TargetWidth = nWidth > 4096 + MapWidth - 4 ? 4096 + MapWidth - 4 : nWidth;
		TargetHeight = nHeight > WateredScreenHeight ? WateredScreenHeight : nHeight;
	}
}

void PrepareAddresses()
{
	if (Version == WWP_11)
	{
		GlobalEatLimit = 768;
		WWPDDinit = EXE.Offset(0x11CBF1);
		WWPDDterm = EXE.Offset(0x10653C);
		WWPGameEndCont = EXE.Offset(0x105308);
		pWormsWnd = (HWND*)EXE.Offset(0x232D80);
		pT17Wnd   = (HWND*)EXE.Offset(0x3F73C8);
		wwpDD = (LPDIRECTDRAW2*)EXE.Offset(0x3F7354);
		WWPCurPosStruct = (PVOID*)EXE.Offset(0x3F84C4);
		pWWPInGame = (BOOL*)EXE.Offset(0x3F8DE8);
	//	RenderGame = (PFVOID)PE.Offset(0x1051EF); //dont enable

		PatchMemByte(EXE.Offset(0x26B9E), 0xEB); //always set the resolution below
		PatchMemDword(EXE.Offset(0x26BB4), SWidth); //settings swidth
		PatchMemDword(EXE.Offset(0x26BC1), SHeight); //settings sheight
		PatchMemDword(EXE.Offset(0x10601D), 0x7FFF); //change max width limit to 32767
		PatchMemDword(EXE.Offset(0x10603F), 0x7FFF); //change max height limit to 32767

		InsertJump((PVOID)EXE.Offset(0x123209), 6, &ProcessWWPWater, IJ_CALL); //WaterInit
		InsertJump((PVOID)EXE.Offset(0x18BA16), 6, &ProcessWWPWater, IJ_CALL); //WaterLastInit
		InsertJump((PVOID)EXE.Offset(0x18EF0B), 6, &ProcessWWPWater, IJ_CALL); //WaterRise

		InsertJump((PVOID)EXE.Offset(0x105302), 6, &ProcessWWPGameEnd);
		InsertJump((PVOID)EXE.Offset(0x106537), 5, &ProcessDDStartup);
	}
	else if (Version == WWP_10)
	{
		GlobalEatLimit = 768;
		WWPDDinit = EXE.Offset(0xFE751);
		WWPDDterm = EXE.Offset(0xE809C);
		WWPGameEndCont = EXE.Offset(0xE6E68);
		pWormsWnd = (HWND*)EXE.Offset(0x2B4CB0);
		pT17Wnd = (HWND*)EXE.Offset(0x47C7A8);
		wwpDD = (LPDIRECTDRAW2*)EXE.Offset(0x47C734);
		WWPCurPosStruct = (PVOID*)EXE.Offset(0x47DA40);
		pWWPInGame = (BOOL*)EXE.Offset(0x199118);

		PatchMemByte(EXE.Offset(0x24954), 0xEB); //always set the resolution below
		PatchMemDword(EXE.Offset(0x2496A), SWidth); //settings swidth
		PatchMemDword(EXE.Offset(0x24977), SHeight); //settings sheight
		PatchMemDword(EXE.Offset(0xE7B7D), 0x7FFF); //change max width limit to 32767
		PatchMemDword(EXE.Offset(0xE7B9F), 0x7FFF); //change max height limit to 32767

		InsertJump((PVOID)EXE.Offset(0x104D69), 6, &ProcessWWPWater, IJ_CALL); //WaterInit
		InsertJump((PVOID)EXE.Offset(0x16D536), 6, &ProcessWWPWater, IJ_CALL); //WaterLastInit
		InsertJump((PVOID)EXE.Offset(0x170A2B), 6, &ProcessWWPWater, IJ_CALL); //WaterRise

		InsertJump((PVOID)EXE.Offset(0xE6E62), 6, &ProcessWWPGameEnd);
		InsertJump((PVOID)EXE.Offset(0xE8097), 5, &ProcessDDStartup);
	}
	else //Worms 2
	{
		//credits for these go to S*natch's and des's patches; labels described by StepS

		LandWaterCriticalZone = EXE.Offset(0x279E7 + 0xC00);
		CavernWaterEatLimit   = EXE.Offset(0x279F4 + 0xC00);
		ActualHeight          = EXE.Offset(0x3328A + 0xC00);
		ActualWidth           = EXE.Offset(0x33292 + 0xC00);
		TopOffset             = EXE.Offset(0x45B36 + 0xC00);
		HorizontalSidesBox    = EXE.Offset(0x45B40 + 0xC00);
		LeftOffset            = EXE.Offset(0x45B4B + 0xC00);
		VerticalSidesBox      = EXE.Offset(0x45B55 + 0xC00);
		RenderFromTop         = EXE.Offset(0x45B71 + 0xC00);
		RenderFromLeft        = EXE.Offset(0x45B76 + 0xC00);

		AL_SWUnk1             = EXE.Offset(0x45A9A + 0xC00);
		AL_WUnk2              = EXE.Offset(0x45AB1 + 0xC00);
		AL_HorizontalSidesBox = EXE.Offset(0x45AD7 + 0xC00);
		AL_TopInfidelBox      = EXE.Offset(0x45ADC + 0xC00);
		AL_LeftOffset         = EXE.Offset(0x45B05 + 0xC00);
		AL_RenderFromTop      = EXE.Offset(0x45B16 + 0xC00);
		AL_RenderFromLeft     = EXE.Offset(0x45B1B + 0xC00);

		//new things discovered by StepS

		GlobalEatLimit = 480;
		pWormsWnd       = (HWND*)EXE.Offset(0x8BCE8);
		pW2DS           = (LPW2DDSTRUCT*)EXE.Offset(0x799C4);
		RenderGame      = (PFVOID)EXE.Offset(0x34750);
		W2PosCoord      = (LPLCOORD)EXE.Offset(0x79478);

		InsertJump((PVOID)EXE.Offset(0x285FE), 9, &ProcessW2Waterrise, IJ_CALL); //WaterRise
	}
}

void PatchW2Mem(DWORD nWidth, DWORD nHeight, bool bMouseForWindow)
{
	GetTargetScreenSize(nWidth, nHeight);

	PatchMemDword(LandWaterCriticalZone, GlobalEatLimit << 16); //in case other reso exepatches broke this value
	PatchMemDword(CavernWaterEatLimit  , GlobalEatLimit << 16);
	PatchMemDword(ActualWidth          , nWidth);
	PatchMemDword(ActualHeight         , nHeight);
	PatchMemDword(RenderFromLeft       , TargetWidth << 16);
	PatchMemDword(RenderFromTop        , TargetHeight << 16);
	PatchMemDword(HorizontalSidesBox   , TargetWidth);
	PatchMemDword(VerticalSidesBox     , TargetHeight);
	PatchMemDword(LeftOffset           , (TargetWidth / 2) << 16);
	PatchMemDword(TopOffset            , (TargetHeight / 2) << 16);

	PatchMemDword(AL_WUnk2             , TargetWidth << 16);
	PatchMemDword(AL_HorizontalSidesBox, TargetWidth);
	PatchMemDword(AL_RenderFromLeft    , TargetWidth << 16);
	PatchMemDword(AL_TopInfidelBox     , TargetHeight);
	PatchMemDword(AL_RenderFromTop     , TargetHeight << 16);
	PatchMemDword(AL_SWUnk1            , (TargetWidth / 2) << 16);
	PatchMemDword(AL_LeftOffset        , (TargetWidth / 2) << 16);

	UpdateCenteredCursor(nWidth, nHeight, bMouseForWindow);
}

BOOL UpdateCenteredCursor(DWORD nWidth, DWORD nHeight, bool bMouseForWindow)
{
	if (GCursPos())
	{
		ScreenCX = GetSystemMetrics(SM_CXSCREEN);
		ScreenCY = GetSystemMetrics(SM_CYSCREEN);

		if (bMouseForWindow && IsWindow(InputWnd()))
		{
			DWORD width, height;
			GetWndSize(InputWnd(), width, height);

			GCursPos()->X = width / 2 > ScreenCX / 2 ? ScreenCX / 2 : width / 2;
			GCursPos()->Y = height / 2 > ScreenCY / 2 ? ScreenCY / 2 : height / 2;
		}
		else
		{
			GCursPos()->X = nWidth / 2 > ScreenCX / 2 ? ScreenCX / 2 : nWidth / 2;
			GCursPos()->Y = nHeight / 2 > ScreenCY / 2 ? ScreenCY / 2 : nHeight / 2;
		}
	}
	else
		return FALSE;
	return TRUE;
}

void SetWWPRenderingDimensions(DWORD nWidth, DWORD nHeight, bool bMouseForWindow)
{
	if (WidthNow && HeightNow)
	{
		PatchMemDword(WidthNow, nWidth);
		PatchMemDword(HeightNow, nHeight);
		UpdateCenteredCursor(nWidth, nHeight, bMouseForWindow);
	}
}

void AdjustWWPRenderer()
{
	GetTargetScreenSize(TWidth, THeight);
	SetWWPRenderingDimensions(TargetWidth, TargetHeight);
}

__declspec(naked) void ProcessW2Waterrise()
{
	//eax: game's calculated spaceheight
	__asm{
		push ecx
		push eax
		push edx
		mov edx, [ecx+27Ch]
		add edx, 0A0h
		shl edx, 10h
		sub edx, [ecx+9DC8h]
		cmp edx, [GlobalEatLimit]
		jge NotFinalBox
		sub eax, [ecx+9DC0h] //Fix the (usually 2) pixel addition to the final camera height
		jmp NoJitter
	NotFinalBox:
		mov edx, [ecx+27Ch]
		shl edx, 10h
		cmp edx, [esi+84h] //Water will rise until [esi+84h >> 16]
		jg NoJitter
		sub eax, 20000h //-2 Delta: TEST to mitigate a very short left-right jitter; may be improved later
	NoJitter:
		sar eax, 10h
		cmp eax, [WateredScreenHeight]
		je JumpOut
		mov [WateredScreenHeight], eax
		push 0
		push [THeight]
		push [TWidth]
		call PatchW2Mem
		add esp, 0Ch
		call CleanupSurfaces
	JumpOut:
		pop edx
		pop eax
		pop ecx
		mov eax, [esi + 74h]
		mov ecx, [eax + 9DCCh]
		ret
	}
}

__declspec(naked) void ProcessWWPWater()
{
	//ecx: game's calculated spaceheight
	__asm{
		push ecx
		cmp ecx, 300h
		jge AtLeast300
		mov ecx, 300h
	AtLeast300:
		cmp ecx, [WateredScreenHeight]
		je JumpOut
		mov [WateredScreenHeight], ecx
		push eax
		push edx
		call AdjustWWPRenderer
		call CleanupSurfaces
		pop edx
		pop eax
	JumpOut:
		pop ecx
		cmp ecx, 300h
		ret
	}
}


__declspec(naked) void ProcessDDStartup()
{
	__asm{
		mov [WidthNow], ecx
		mov [HeightNow], ecx
		add [WidthNow], 3568h
		add [HeightNow], 356Ch
		call WWPDDinit
		push eax
		push ecx
		push edx
		call CavernCheck
		call AdjustWWPRenderer
		pop edx
		pop ecx
		pop eax
		jmp WWPDDterm
	}
}

__declspec(naked) void ProcessWWPGameEnd()
{
	__asm{
		push eax
		push ecx
		push edx
		call InitializeScreenSize
		pop edx
		pop ecx
		pop eax
		push ebp
		mov ebp, esp
		sub esp, 0Ch
		jmp WWPGameEndCont
	}
}

void InitializeScreenSize()
{
	WidthNow = 0;
	HeightNow = 0;

	TargetWidth = SWidth;  TargetHeight = SHeight;
	LastWidth   = SWidth;  LastHeight   = SHeight;
	TWidth      = SWidth;  THeight      = SHeight;
	DTWidth     = SWidth;  DTHeight     = SHeight;
	DDif = DTHeight / DTWidth;

	AeWidth  = (DWORD)(DTWidth / 1.5);
	AeHeight = (DWORD)(DTHeight / 1.5);
}
