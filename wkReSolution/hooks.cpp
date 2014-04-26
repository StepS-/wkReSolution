
#include <windows.h>
#include "hooks.h"
#include "w2res.h"
#include "madCHook/madCHook.h"
#pragma comment (lib, "madCHook/madCHook.lib")

HWND W2Wnd;
HHOOK wHook;
LPDIRECTDRAW DDObj;
LPDIRECTDRAWSURFACE GDISurf;
PVOID W2DDHookStart, W2DDHookNext;
PVOID W2DDSizeStruct;
BOOL ModifiedSurfaces;

SHORT TWidth, THeight, LastWidth, LastHeight;

__declspec(naked) void EndMadHook()
{
	__asm
	{
		push eax
		push ebx
		mov eax, [esp + 0Ch]
		mov ebx, [eax + 1]
		mov [esp + 0Ch], ebx
		lock and dword ptr [eax + 1], 0
		pop ebx
		pop eax
		ret
	}
}

__declspec(naked) void UpdateW2DDSizeStruct()
{
	__asm
	{
		push eax
		push ebx
		push ecx
		mov ax, [TargetWidth]
		mov bx, [TargetHeight]
		mov ecx, [W2DDSizeStruct]
		mov [ecx + 8h], ax
		mov [ecx + 0Ch], bx
		pop ecx
		pop ebx
		pop eax
		ret
	}
}

__declspec(naked) void W2DDInitHook()
{
	__asm
	{
		call EndMadHook
		mov [W2DDSizeStruct], eax
		call UpdateW2DDSizeStruct
		jmp W2DDHookNext
	}
}

//HACK
HRESULT WINAPI EnumResize(LPDIRECTDRAWSURFACE pSurface, LPDDSURFACEDESC lpSurfaceDesc, LPVOID lpContext)
{
	BOOL bRequiredSurface  = (!CVal(lpSurfaceDesc->dwFlags, DDSD_CKSRCBLT) && lpSurfaceDesc->dwWidth == LastWidth && lpSurfaceDesc->dwHeight == LastHeight);
	BOOL bThirtyTwoSurface = ( CVal(lpSurfaceDesc->dwFlags, DDSD_CKSRCBLT) && lpSurfaceDesc->dwWidth == LastWidth && lpSurfaceDesc->dwHeight == 32);
	BOOL bPrimary          = ( CVal(lpSurfaceDesc->ddsCaps.dwCaps, DDSCAPS_PRIMARYSURFACE));

	if ((bRequiredSurface || bThirtyTwoSurface) && !bPrimary)
	{
		LONG lsz  = sizeof(LONG);
		LONG lmod = 8;
		if (lpSurfaceDesc->lPitch != RoundUp(lpSurfaceDesc->dwWidth, 8))
			lmod  = 2;

		DWORD   dwSurfPtr    = *(PDWORD)((DWORD)pSurface + lsz);
		DWORD   dwDataPtr    = *(PDWORD)dwSurfPtr;
		DWORD   dwInfoPtr    = *(PDWORD)(dwSurfPtr + lsz * 2);
		DWORD   dwOldPitchM  = lpSurfaceDesc->lPitch / RoundUp(lpSurfaceDesc->dwWidth, lmod);
		DWORD   dwNewPitch   = RoundUp(TWidth, lmod) * dwOldPitchM;
		DWORD   dwNewMemSize = dwNewPitch * THeight;
		PDWORD lpSurfMemSize = (PDWORD)(dwDataPtr + 0x10);
		PDWORD lpSurfMemAddr = (PDWORD)(dwDataPtr + 0xA8);
		PWORD  lpDataWidth   = (PWORD)(dwDataPtr + 0xB2);
		PWORD  lpDataHeight  = (PWORD)(dwDataPtr + 0xB0);
		PDWORD lpDataPitch   = (PDWORD)(dwDataPtr + 0xAC);
		PDWORD lpInfoWidth   = (PDWORD)(dwInfoPtr + 0x28);
		PDWORD lpInfoHeight  = (PDWORD)(dwInfoPtr + 0x2C);
		PDWORD lpInfoPitch   = (PDWORD)(dwInfoPtr + 0x50);
		PDWORD lpInfoMemAddr = (PDWORD)(dwInfoPtr + 0x4C);

		HLOCAL MemAlloc = LocalHandle((LPCVOID)(*lpSurfMemAddr - lsz * 2));

		if (bThirtyTwoSurface)
			dwNewMemSize = dwNewPitch * 32;

		if (MemAlloc = LocalReAlloc(MemAlloc, dwNewMemSize + lsz * 2, LMEM_MOVEABLE))
		{
			PVOID NewMemPtr = LocalLock(MemAlloc);
			*lpSurfMemSize  = dwNewMemSize;
			*lpSurfMemAddr  = (DWORD)NewMemPtr + lsz * 2;
			*lpDataWidth    = TWidth;
			*lpDataPitch    = dwNewPitch;
			*lpInfoWidth    = TWidth;
			*lpInfoPitch    = dwNewPitch;
			*lpInfoMemAddr  = *lpSurfMemAddr;

			if (!bThirtyTwoSurface)
			{
				*lpDataHeight = THeight;
				*lpInfoHeight = THeight;

				//cleaning the excess picture data
				GetTargetScreenSize(TWidth, THeight);
				if (TargetHeight < THeight)
					memset((PVOID)(*lpSurfMemAddr + dwNewPitch*TargetHeight), 0, dwNewPitch * (THeight - TargetHeight));
				if (TargetWidth < TWidth)
				for (int i = 0; i < TargetHeight; i++)
					memset((PVOID)(*lpSurfMemAddr + i*dwNewPitch + TargetWidth*dwOldPitchM), 0, dwNewPitch - TargetWidth*dwOldPitchM);
			}
			ModifiedSurfaces++;
			LocalUnlock(MemAlloc);
		}
	}
	return DDENUMRET_OK;
}

LRESULT __declspec(dllexport) CALLBACK CallWndProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	if (nCode == HC_ACTION)
	{
		CWPSTRUCT* pwp = (CWPSTRUCT*)lParam;
		if (pwp->message == WM_WINDOWPOSCHANGED)
		{
			if (pwp->hwnd == W2Wnd)
			{
				LPWINDOWPOS lwp = (LPWINDOWPOS)(pwp->lParam);
				if (!CVal(lwp->flags, SWP_NOSIZE) && !CVal(lwp->flags, SWP_NOCOPYBITS) && !CVal(lwp->flags, SWP_NOSENDCHANGING))
				if (DDObj)
				if (SUCCEEDED(DDObj->GetGDISurface(&GDISurf)))
				{
					RECT W2rect;
					GetClientRect(W2Wnd, &W2rect);
					SHORT width = (SHORT)(W2rect.right - W2rect.left);
					SHORT height = (SHORT)(W2rect.bottom - W2rect.top);
					if (width > 0 && height > 0)
					{
						TWidth = width;
						THeight = height;
						ModifiedSurfaces = 0;
						if (LastWidth != TWidth || LastHeight != THeight)
						if (SUCCEEDED(DDObj->EnumSurfaces(DDENUMSURFACES_DOESEXIST | DDENUMSURFACES_ALL, NULL, GDISurf, EnumResize)))
						{
							LastWidth = TWidth;
							LastHeight = THeight;
							PatchMem(TWidth, THeight);
							if (W2DDSizeStruct)
								UpdateW2DDSizeStruct();
							if (ModifiedSurfaces && ProgressiveResize)
								RenderGame(); //Experimental: rerender the scene right after resizing
						}
					}
				}
			}
		}
	}

	return CallNextHookEx(wHook, nCode, wParam, lParam);
}

HRESULT(WINAPI *DirectDrawCreateNext)(GUID FAR *lpGUID, LPDIRECTDRAW FAR *lplpDD, IUnknown FAR *pUnkOuter);
HRESULT WINAPI DirectDrawCreateHook(GUID FAR *lpGUID, LPDIRECTDRAW FAR *lplpDD, IUnknown FAR *pUnkOuter)
{
	HRESULT result = DirectDrawCreateNext(lpGUID, lplpDD, pUnkOuter);
	if (SUCCEEDED(result))
		DDObj = (LPDIRECTDRAW)(*lplpDD);
	return result;
}

HWND(WINAPI *CreateWindowExANext)(DWORD dwExStyle, LPCSTR lpClassName, LPCSTR lpWindowName, DWORD dwStyle, int x, int y,
	int nWidth, int nHeight, HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam);
HWND WINAPI CreateWindowExAHook(DWORD dwExStyle, LPCSTR lpClassName, LPCSTR lpWindowName, DWORD dwStyle, int x, int y,
	int nWidth, int nHeight, HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam)
{
	HWND Wnd = CreateWindowExANext(dwExStyle, lpClassName, lpWindowName, dwStyle, x, y, nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam);
	if (lpClassName && !W2Wnd)
	if (!strcmp(lpClassName, "worms2"))
		W2Wnd = Wnd;
	return Wnd;
}

void InstallHooks()
{
	if (AllowResize)
	{
		wHook = SetWindowsHookEx(WH_CALLWNDPROC, (HOOKPROC)CallWndProc, 0, GetCurrentThreadId());
		HookAPI("ddraw.dll", "DirectDrawCreate", DirectDrawCreateHook, (PVOID*)&DirectDrawCreateNext, 0);
		HookAPI("user32.dll", "CreateWindowExA", CreateWindowExAHook, (PVOID*)&CreateWindowExANext, 0);
	}
	HookCode(W2DDHookStart, W2DDInitHook, (PVOID*)&W2DDHookNext, 0);
}

void UninstallHooks()
{
	if (AllowResize)
		UnhookWindowsHookEx(wHook);
	FinalizeMadCHook();
}
