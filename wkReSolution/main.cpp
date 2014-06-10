
#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <stdio.h>
#include "main.h"
#include "hooks.h"
#include "w2res.h"
#include "misc_tools.h"

void LoadConfig()
{
	GetPathUnderExeA(Config, "W2.ini");

	ScreenCX = (SHORT)GetSystemMetrics(SM_CXSCREEN);
	ScreenCY = (SHORT)GetSystemMetrics(SM_CYSCREEN);

	WinMinWidth = GetSystemMetrics(SM_CXMIN) - GetSystemMetrics(SM_CXSIZEFRAME) * 2;

	SWidth                = GetPrivateProfileIntA("Resolution", "ScreenWidth", -1, Config);
	SHeight               = GetPrivateProfileIntA("Resolution", "ScreenHeight", -1, Config);
	OfflineCavernFloodFix = GetPrivateProfileIntA("Resolution", "OfflineCavernFloodFix", -1, Config);

	AllowResize           = GetPrivateProfileIntA("Resizing", "Enable", -1, Config);
	ProgressiveResize     = GetPrivateProfileIntA("Resizing", "ProgressiveUpdate", -1, Config);
	AltEnter              = GetPrivateProfileIntA("Resizing", "AltEnter", -1, Config);

	AllowZoom             = GetPrivateProfileIntA("Zooming", "Enable", -1, Config);
	UseMouseWheel         = GetPrivateProfileIntA("Zooming", "UseMouseWheel", -1, Config);
	UseKeyboardZoom       = GetPrivateProfileIntA("Zooming", "UseKeyboardZoom", -1, Config);

	if (SWidth <= 0 || SHeight <= 0)
	{
		WritePrivateProfileIntA("Resolution", "ScreenWidth", SWidth = ScreenCX, Config);
		WritePrivateProfileIntA("Resolution", "ScreenHeight", SHeight = ScreenCY, Config);
	}

	if (OfflineCavernFloodFix < 0)
		WritePrivateProfileIntA("Resolution", "OfflineCavernFloodFix", OfflineCavernFloodFix = 1, Config);
	if (AllowResize < 0)
		WritePrivateProfileIntA("Resizing", "Enable", AllowResize = 1, Config);
	if (ProgressiveResize < 0)
		WritePrivateProfileIntA("Resizing", "ProgressiveUpdate", ProgressiveResize = 1, Config);
	if (AltEnter < 0)
		WritePrivateProfileIntA("Resizing", "AltEnter", AltEnter = 1, Config);
	if (AllowZoom < 0)
		WritePrivateProfileIntA("Zooming", "Enable", AllowZoom = 1, Config);
	if (UseMouseWheel < 0)
		WritePrivateProfileIntA("Zooming", "UseMouseWheel", UseMouseWheel = 1, Config);
	if (UseKeyboardZoom < 0)
		WritePrivateProfileIntA("Zooming", "UseKeyboardZoom", UseKeyboardZoom = 1, Config);

	TargetWidth = SWidth;  TargetHeight = SHeight;
	LastWidth   = SWidth;  LastHeight   = SHeight;
	TWidth      = SWidth;  THeight      = SHeight;
	DTWidth     = SWidth;  DTHeight     = SHeight;
	DDif = DTHeight / DTWidth;

	AeWidth  = (SHORT)(DTWidth / 1.5);
	AeHeight = (SHORT)(DTHeight / 1.5);

	if (OfflineCavernFloodFix)
		GlobalEatLimit = 854;
	else
		GlobalEatLimit = 480;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD dwReason, LPVOID lpReserved)
{
	if (dwReason == DLL_PROCESS_ATTACH)
	{
		if (!CheckVersion())
		{
			MessageBoxA(NULL,
				"Sorry, but your Worms2.exe version has to be 1.05 for wkReSolution to work. "
				"Please patch your game to 1.05 and try again.", "ReSolution error",
				MB_OK | MB_ICONERROR);
			return 1;
		}

		LoadConfig();
		CavernCheck();
		GetAddresses();
		UnprotectAddresses();
		PatchMem(SWidth, SHeight);
		InstallHooks();

	}
	else if (dwReason == DLL_PROCESS_DETACH)
	{
		UninstallHooks();
	}

	return 1;
}

