
#include <windows.h>
#include <stdio.h>
#include "w2res.h"
#include "misc_tools.h"

bool Cavern;
BYTE Version;
CHAR Config[MAX_PATH], LandFile[MAX_PATH];

SHORT SWidth, SHeight, GlobalEatLimit, TargetWidth, TargetHeight;
SHORT ScreenX, ScreenY;
BOOL OfflineCavernFloodFix, AllowResize, ProgressiveResize;
DWORD ActualWidth, HorizontalSidesBox, RenderFromLeft;
DWORD LandWaterCriticalZone, CavernWaterEatLimit, ActualHeight, HUnk4, HUnk5, RenderFromTop, VerticalSidesBox;
DWORD LeftOffset, CenterCursorX, CenterCursorY;
DWORD AL_WUnk2, AL_HorizontalSidesBox, AL_RenderFromLeft, AL_RenderFromTop, AL_TopInfidelBox, AL_SWUnk1, AL_LeftOffset;
DWORD TopOffset;

PFVOID RenderGame;

BYTE CheckVersion()
{
	DWORD PETime = GetPETimestampA(0);
	if (PETime >= 0x352118A5 && PETime <= 0x352118FD)
		return Version = 1;
	return Version = 0;
}

BOOL CavernCheck()
{
	FILE* land;
	char cavc;
	Cavern = false;
	if (fopen_s(&land, GetPathUnderExeA(LandFile, "Data\\land.dat"), "r") == ERROR_SUCCESS)
	{
		fseek(land, 0x10, SEEK_SET);
		cavc = fgetc(land);
		if (cavc)
			Cavern = true;
		fclose(land);
	}
	else
		MessageBoxA(NULL,
		"Warning: failed to open the \"Data\\land.dat\" file. "
		"Your game will most likely crash.", "ReSolution warning",
		MB_OK | MB_ICONWARNING);

	return Cavern;
}

void GetTargetScreenSize(SHORT nWidth, SHORT nHeight)
{
	if (Cavern)
	{
		TargetWidth  = nWidth > 1920 ? 1920 : nWidth;
		TargetHeight = nHeight > 856 ? 856  : nHeight;
	}
	else
	{
		TargetWidth  = nWidth  > 6012 ? 6012 : nWidth;
		TargetHeight = nHeight > 2902 ? 2902 : nHeight;
	}
}

void GetAddresses()
{
	//credits for these go to S*natch and des; labels described by StepS

	LandWaterCriticalZone = 0x000279E9 + MemOffset(0xC00);
	CavernWaterEatLimit   = 0x000279F6 + MemOffset(0xC00);
	ActualHeight          = 0x0003328A + MemOffset(0xC00);
	ActualWidth           = 0x00033292 + MemOffset(0xC00);
	TopOffset             = 0x00045B38 + MemOffset(0xC00);
	HorizontalSidesBox    = 0x00045B40 + MemOffset(0xC00);
	LeftOffset            = 0x00045B4D + MemOffset(0xC00);
	VerticalSidesBox      = 0x00045B55 + MemOffset(0xC00);
	RenderFromTop         = 0x00045B73 + MemOffset(0xC00);
	RenderFromLeft        = 0x00045B78 + MemOffset(0xC00);

	AL_SWUnk1             = 0x00045A9C + MemOffset(0xC00);
	AL_WUnk2              = 0x00045AB3 + MemOffset(0xC00);
	AL_HorizontalSidesBox = 0x00045AD7 + MemOffset(0xC00);
	AL_TopInfidelBox      = 0x00045ADC + MemOffset(0xC00);
	AL_LeftOffset         = 0x00045B07 + MemOffset(0xC00);
	AL_RenderFromTop      = 0x00045B18 + MemOffset(0xC00);
	AL_RenderFromLeft     = 0x00045B1D + MemOffset(0xC00);

	//	HUnk4             = 0x000363F6 + MemOffset(0xC00);
	//	HUnk5             = 0x0004000C + MemOffset(0xC00);

	//new things discovered by StepS

	CenterCursorX = 0x00077878 + MemOffset(0x1C00);
	CenterCursorY = 0x0007787C + MemOffset(0x1C00);
}

void UnprotectAddresses()
{
	Unprotect(ActualWidth);
	Unprotect(CavernWaterEatLimit);
	Unprotect(TopOffset);
}

void PatchMem(SHORT nWidth, SHORT nHeight)
{
	GetTargetScreenSize(nWidth, nHeight);

	*(PWORD)LandWaterCriticalZone = GlobalEatLimit;
	*(PWORD)CavernWaterEatLimit   = GlobalEatLimit;
	*(PWORD)ActualWidth           = nWidth;
	*(PWORD)ActualHeight          = nHeight;
	*(PWORD)RenderFromLeft        = TargetWidth;
	*(PWORD)RenderFromTop         = TargetHeight;
	*(PWORD)HorizontalSidesBox    = TargetWidth;
	*(PWORD)VerticalSidesBox      = TargetHeight;
	*(PWORD)LeftOffset            = TargetWidth / 2;
	*(PWORD)TopOffset             = TargetHeight / 2;

	*(PWORD)AL_WUnk2              = nWidth;
	*(PWORD)AL_HorizontalSidesBox = TargetWidth;
	*(PWORD)AL_RenderFromLeft     = TargetWidth;
	*(PWORD)AL_TopInfidelBox      = TargetHeight;
	*(PWORD)AL_RenderFromTop      = nHeight;
	*(PWORD)AL_SWUnk1             = TargetWidth / 2;
	*(PWORD)AL_LeftOffset         = TargetWidth / 2;

	*(PWORD)CenterCursorX = nWidth / 2 > ScreenX / 2 ? ScreenX / 2 : nWidth / 2;
	*(PWORD)CenterCursorY = nHeight / 2 > ScreenY / 2 ? ScreenY / 2 : nHeight / 2;

	//  *(PWORD)HUnk4 = nHeight;
	//  *(PWORD)HUnk5 = nHeight;
	//  unknown, readonly values
}
