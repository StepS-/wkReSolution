
#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include "main.h"
#include "hooks.h"
#include "w2res.h"
#include "misc_tools.h"

void LoadConfig()
{
	GetPathUnderExeA(Config, "W2.ini");

	SWidth = GetPrivateProfileIntA("Resolution", "ScreenWidth", -1, Config);
	SHeight = GetPrivateProfileIntA("Resolution", "ScreenHeight", -1, Config);
	OfflineCavernFloodFix = GetPrivateProfileIntA("Resolution", "OfflineCavernFloodFix", -1, Config);

	ScreenX = (SHORT)GetSystemMetrics(SM_CXSCREEN);
	ScreenY = (SHORT)GetSystemMetrics(SM_CYSCREEN);

	if (OfflineCavernFloodFix < 0)
	{
		OfflineCavernFloodFix = 1;
		WritePrivateProfileIntA("Resolution", "OfflineCavernFloodFix", 1, Config);
	}

	if (SWidth <= 0 || SHeight <= 0)
	{
		SWidth = ScreenX;
		SHeight = ScreenY;
		WritePrivateProfileIntA("Resolution", "ScreenWidth", SWidth, Config);
		WritePrivateProfileIntA("Resolution", "ScreenHeight", SHeight, Config);
	}

	TargetWidth = SWidth;
	TargetHeight = SHeight;
	LastWidth = SWidth;
	LastHeight = SHeight;
	TWidth = SWidth;
	THeight = SHeight;

	W2DDHookStart = (PVOID)MemOffset(0x33E9F);
	RenderGame    = (PFVOID)MemOffset(0x34750);

	if (OfflineCavernFloodFix)
		GlobalEatLimit = 854;
	else
		GlobalEatLimit = 480;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
	if (ul_reason_for_call == DLL_PROCESS_ATTACH)
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
	else if (ul_reason_for_call == DLL_PROCESS_DETACH)
	{
		UninstallHooks();
	}

	return 1;
}

