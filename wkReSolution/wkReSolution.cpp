
#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <stdio.h>

bool Cavern;
BYTE Version;
CHAR Config[MAX_PATH], LandFile[MAX_PATH];
HHOOK wHook;

SHORT SWidth, SHeight, GlobalEatLimit, TargetWidth, TargetHeight;
SHORT ScreenX, ScreenY;
DWORD OfflineCavernFloodFix;
DWORD ActualWidth, HorizontalSidesBox, RenderFromLeft;
DWORD LandWaterCriticalZone, CavernWaterEatLimit, ActualHeight, HUnk4, HUnk5, RenderFromTop, VerticalSidesBox;
DWORD LeftOffset, CenterCursorX, CenterCursorY;
DWORD AL_WUnk2, AL_HorizontalSidesBox, AL_RenderFromLeft, AL_RenderFromTop, AL_TopInfidelBox, AL_SWUnk1, AL_LeftOffset;
DWORD TopOffset;

DWORD GetPETimestamp(LPCTSTR lpModuleName)
{
	DWORD ImageBase = (DWORD)GetModuleHandle(lpModuleName);
	DWORD PEOffset = *(DWORD*)(ImageBase + 0x3C);
	return *(DWORD*)(ImageBase + PEOffset + 0x08);
}

BYTE CheckVersion()
{
	DWORD PETime = GetPETimestamp(0);
	if (PETime >= 0x352118A5 && PETime <= 0x352118FD)
		return Version = 1;
	return Version = 0;
}

BOOL WritePrivateProfileIntA(LPCSTR lpAppName, LPCSTR lpKeyName, int nInteger, LPCSTR lpFileName)
{
	CHAR lpString[32];
	sprintf_s(lpString, "%d", nInteger);
	return WritePrivateProfileStringA(lpAppName, lpKeyName, lpString, lpFileName);
}

LPSTR GetPathUnderExeA(LPSTR OutBuf, LPCSTR FileName)
{
	GetModuleFileNameA(NULL, OutBuf, MAX_PATH);
	CHAR* dirend = strrchr(OutBuf, '\\') + 1;
	strcpy_s(dirend, MAX_PATH, FileName);
	return OutBuf;
}

BOOL Unprotect(ULONG addr, SIZE_T dwSize = sizeof(WORD))
{
	DWORD uselessDword;
	return VirtualProtect((void*)addr, dwSize, PAGE_READWRITE, &uselessDword);
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
		TargetWidth = nWidth > 1920 ? 1920 : nWidth;
		TargetHeight = nHeight > 856 ? 856 : nHeight;
	}
	else
	{
		TargetWidth = nWidth > 6012 ? 6012 : nWidth;
		TargetHeight = nHeight > 2902 ? 2902 : nHeight;
	}
}

void GetAddresses()
{
	//credits for these go to S*natch and des; labels described by StepS

	LandWaterCriticalZone = 0x000279E9 + 0x400C00;
	CavernWaterEatLimit = 0x000279F6 + 0x400C00;
	ActualHeight = 0x0003328A + 0x400C00;
	ActualWidth = 0x00033292 + 0x400C00;
	TopOffset = 0x00045B38 + 0x400C00;
	HorizontalSidesBox = 0x00045B40 + 0x400C00;
	LeftOffset = 0x00045B4D + 0x400C00;
	VerticalSidesBox = 0x00045B55 + 0x400C00;
	RenderFromTop = 0x00045B73 + 0x400C00;
	RenderFromLeft = 0x00045B78 + 0x400C00;

	AL_SWUnk1 = 0x00045A9C + 0x400C00;
	AL_WUnk2 = 0x00045AB3 + 0x400C00;
	AL_HorizontalSidesBox = 0x00045AD7 + 0x400C00;
	AL_TopInfidelBox = 0x00045ADC + 0x400C00;
	AL_LeftOffset = 0x00045B07 + 0x400C00;
	AL_RenderFromTop = 0x00045B18 + 0x400C00;
	AL_RenderFromLeft = 0x00045B1D + 0x400C00;

	//	HUnk4                 = 0x000363F6 + 0x400C00;
	//	HUnk5                 = 0x0004000C + 0x400C00;

	//new things discovered by StepS

	CenterCursorX = 0x00077878 + 0x401C00;
	CenterCursorY = 0x0007787C + 0x401C00;
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

	*(WORD*)LandWaterCriticalZone = GlobalEatLimit;
	*(WORD*)CavernWaterEatLimit = GlobalEatLimit;
	*(WORD*)ActualWidth = SWidth;
	*(WORD*)ActualHeight = SHeight;
	*(WORD*)RenderFromLeft = TargetWidth;
	*(WORD*)RenderFromTop = TargetHeight;
	*(WORD*)HorizontalSidesBox = TargetWidth;
	*(WORD*)VerticalSidesBox = TargetHeight;
	*(WORD*)LeftOffset = TargetWidth / 2;
	*(WORD*)TopOffset = TargetHeight / 2;

	*(WORD*)CenterCursorX = SWidth / 2 > ScreenX / 2 ? ScreenX / 2 : SWidth / 2;
	*(WORD*)CenterCursorY = SHeight / 2 > ScreenY / 2 ? ScreenY / 2 : SHeight / 2;

	*(WORD*)AL_WUnk2 = SWidth;
	*(WORD*)AL_HorizontalSidesBox = TargetWidth;
	*(WORD*)AL_RenderFromLeft = TargetWidth;
	*(WORD*)AL_TopInfidelBox = TargetHeight;
	*(WORD*)AL_RenderFromTop = SHeight;
	*(WORD*)AL_SWUnk1 = TargetWidth / 2;
	*(WORD*)AL_LeftOffset = TargetWidth / 2;

	//  *(WORD*)HUnk4 = SHeight;
	//  *(WORD*)HUnk5 = SHeight;
	//  unknown, readonly values
}

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

	if (OfflineCavernFloodFix)
		GlobalEatLimit = 854;
	else
		GlobalEatLimit = 480;
}

LRESULT __declspec(dllexport)__stdcall  CALLBACK CallWndProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	if (nCode == HC_ACTION)
	{
		CWPSTRUCT* pwp = (CWPSTRUCT*)lParam;

		if (pwp->message == WM_EXITSIZEMOVE)
		{
			if (HWND W2Wnd = FindWindow("Worms2", NULL))
			{
				if (pwp->hwnd == W2Wnd)
				{
					RECT W2rect;
					GetClientRect(W2Wnd, &W2rect);
					SHORT width = (SHORT)(W2rect.right - W2rect.left);
					SHORT height = (SHORT)(W2rect.bottom - W2rect.top);
					PatchMem(width, height);
				}
			}
		}
	}

	return CallNextHookEx(wHook, nCode, wParam, lParam);
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

		wHook = SetWindowsHookEx(WH_CALLWNDPROC, (HOOKPROC)CallWndProc, hModule, GetCurrentThreadId());
	}
	else if (ul_reason_for_call == DLL_PROCESS_DETACH)
	{
		UnhookWindowsHookEx(wHook);
	}

	return 1;
}

