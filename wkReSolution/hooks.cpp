
#include <windows.h>
#include "hooks.h"
#include "w2res.h"

HHOOK wHook, kHook, mHook;
BOOL ModifiedSurfaces;

DWORD TWidth, THeight, LastWidth, LastHeight;
DOUBLE DTWidth, DTHeight, DDif;

BOOL DZoom(DOUBLE& dCX, DOUBLE& dCY, DOUBLE dDif, SHORT sDelta)
{
	BOOL result = 0;

	DOUBLE ddCX = dCX + sDelta;
	DOUBLE ddCY = dCY + sDelta * dDif;

	if ((sDelta > 0 && ddCX <= 32767 && ddCY <= 32767) || (sDelta < 0 && ddCX >= WinMinWidth && ddCY > 0))
	{
		dCX = ddCX;
		dCY = ddCY;
		result = 1;
	}
	return result;
}

BOOL ReNormalizeBuffers()
{
	BOOL result = 0;

	DWORD width, height;
	GetWndSize(WormsWnd(), width, height);

	if (HandleBufferResize(width, height))
	{
		DTWidth = width;
		DTHeight = height;
		DDif = DTHeight / DTWidth;
		result = 1;
	}

	return result;
}

BOOL CleanupSurfaces()
{
	BOOL result = 0;

	if (DDObj())
	{
		DDSURFACEDESC prefDesc;
		prefDesc.dwSize = sizeof(DDSURFACEDESC);
		prefDesc.dwFlags = DDSD_CAPS;
		prefDesc.ddsCaps.dwCaps = 0x8A00;
		if (SUCCEEDED(DDObj()->EnumSurfaces(DDENUMSURFACES_DOESEXIST | DDENUMSURFACES_NOMATCH, &prefDesc, NULL, EnumResize)))
			result = TRUE;
	}
	return result;
}

BOOL HandleBufferResize(DWORD nWidth, DWORD nHeight, bool bRedraw)
{
	BOOL result = 0;

	if (DDObj() && nWidth <= 32767 && nHeight <= 32767 && nWidth && nHeight)
	{
		TWidth = nWidth;
		THeight = nHeight;
		ModifiedSurfaces = 0;
		DDSURFACEDESC prefDesc;
		prefDesc.dwSize = sizeof(DDSURFACEDESC);
		prefDesc.dwFlags = DDSD_CAPS;
		prefDesc.ddsCaps.dwCaps = 0x8A00;
		if (LastWidth != TWidth || LastHeight != THeight)
		if (SUCCEEDED(DDObj()->EnumSurfaces(DDENUMSURFACES_DOESEXIST | DDENUMSURFACES_NOMATCH, &prefDesc, NULL, EnumResize)))
		{
			LastWidth = TWidth;
			LastHeight = THeight;
			if (WWP)
			{
				if (IsWindow(T17Wnd))
				{
					RECT WWPRect;
					if (GetClientRect(WormsWnd(), &WWPRect))
						SetWindowPos(T17Wnd, NULL, 0, 0, WWPRect.right, WWPRect.bottom, SWP_NOMOVE | SWP_NOZORDER);
				}
				GetTargetScreenSize(TWidth, THeight);
				SetWWPRenderingDimensions(TargetWidth, TargetHeight, true);
			}
			else
			{
				PatchW2Mem(TWidth, THeight, true);
				W2DS->RenderWidth = TargetWidth;
				W2DS->RenderHeight = TargetHeight;
				if (ModifiedSurfaces && (ProgressiveResize || bRedraw))
					RenderGame(); //Experimental: rerender the scene right after resizing
			}
			result = 1;
		}
	}

	return result;
}

//HACK
HRESULT WINAPI EnumResize(LPDIRECTDRAWSURFACE pSurface, LPDDSURFACEDESC lpSurfaceDesc, LPVOID)
{
	BOOL bRequiredSurface = (!!!(lpSurfaceDesc->dwFlags & DDSD_CKSRCBLT) && lpSurfaceDesc->dwWidth == LastWidth && lpSurfaceDesc->dwHeight == LastHeight);
	BOOL bPrimary          = (!!(lpSurfaceDesc->ddsCaps.dwCaps & DDSCAPS_PRIMARYSURFACE));

	if (bRequiredSurface && !bPrimary)
	{
		LONG lsz  = sizeof(LONG);
		LONG lmod = 8;
		if ((DWORD)lpSurfaceDesc->lPitch != RoundUp(lpSurfaceDesc->dwWidth, 8))
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

		if ((MemAlloc = LocalReAlloc(MemAlloc, dwNewMemSize + lsz * 2, LMEM_MOVEABLE)) != 0)
		{
			PVOID NewMemPtr = LocalLock(MemAlloc);
			if (NewMemPtr)
			{
				__try {
					*lpSurfMemSize = dwNewMemSize;
					*lpSurfMemAddr = (DWORD)NewMemPtr + lsz * 2;
					*lpDataWidth = LOWORD(TWidth);
					*lpDataPitch = dwNewPitch;
					*lpInfoWidth = TWidth;
					*lpInfoPitch = dwNewPitch;
					*lpInfoMemAddr = *lpSurfMemAddr;

					*lpDataHeight = LOWORD(THeight);
					*lpInfoHeight = THeight;

					//cleanup
					memset((PVOID)(*lpSurfMemAddr), 0, dwNewMemSize);

					ModifiedSurfaces++;
				}
				__finally {
					LocalUnlock(MemAlloc);
				}
			}
		}
	}
	return DDENUMRET_OK;
}

HRESULT WINAPI EnumCleanup(LPDIRECTDRAWSURFACE pSurface, LPDDSURFACEDESC lpSurfaceDesc, LPVOID)
{
	BOOL bRequiredSurface = (!!!(lpSurfaceDesc->dwFlags & DDSD_CKSRCBLT) && lpSurfaceDesc->dwWidth == LastWidth && lpSurfaceDesc->dwHeight == LastHeight);
	BOOL bPrimary = (!!(lpSurfaceDesc->ddsCaps.dwCaps & DDSCAPS_PRIMARYSURFACE));

	if (bRequiredSurface && !bPrimary)
	{
		DWORD  dwSurfPtr = *(PDWORD)((DWORD)pSurface + sizeof(LONG));
		DWORD  dwDataPtr = *(PDWORD)dwSurfPtr;
		PDWORD lpSurfMemSize = (PDWORD)(dwDataPtr + 0x10);
		PDWORD lpSurfMemAddr = (PDWORD)(dwDataPtr + 0xA8);

		HLOCAL MemAlloc = LocalHandle((LPCVOID)(*lpSurfMemAddr - sizeof(LONG)* 2));
		if (LocalLock(MemAlloc))
		{
			__try{
				memset((PVOID)(*lpSurfMemAddr), 0, *lpSurfMemSize);
			}
			__finally{
				LocalUnlock(MemAlloc);
			}
		}
	}
}

BOOL WheelZoom(SHORT sDelta)
{
	if (sDelta != 0)
	{
		if (DZoom(DTWidth, DTHeight, DDif, -sDelta))
			return HandleBufferResize((DWORD)DTWidth, (DWORD)DTHeight);
	}
	return 0;
}

LRESULT CALLBACK CallWndProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	if (nCode == HC_ACTION && InGame())
	{
		CWPSTRUCT* pwp = (CWPSTRUCT*)lParam;
		if (pwp->message == WM_WINDOWPOSCHANGED)
		{
			if (pwp->hwnd == WormsWnd())
			{
				LPWINDOWPOS lwp = (LPWINDOWPOS)(pwp->lParam);
				if (!!!(lwp->flags & SWP_NOSIZE) && !!!(lwp->flags & SWP_NOCOPYBITS) && !!!(lwp->flags & SWP_NOSENDCHANGING))
				{
					ReNormalizeBuffers();
				}
			}
		}
		else if (pwp->message == WM_MOUSEWHEEL && UseTouchscreenZoom)
		{
			if (!!(pwp->wParam & MK_CONTROL))
			{
				WheelZoom(GET_WHEEL_DELTA_WPARAM(pwp->wParam));
			}
		}
	}

	return CallNextHookEx(wHook, nCode, wParam, lParam);
}

LRESULT CALLBACK MouseProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	if (nCode == HC_ACTION && InGame())
	{
		if (wParam == WM_MOUSEWHEEL)
		{
			LPMOUSEHOOKSTRUCTEX lpWheelInf = (LPMOUSEHOOKSTRUCTEX)lParam;
			WheelZoom(GET_WHEEL_DELTA_WPARAM(lpWheelInf->mouseData));
		}
		else if (wParam == WM_MBUTTONDOWN)
		{
			ReNormalizeBuffers();
		}
	}
	return CallNextHookEx(mHook, nCode, wParam, lParam);
}

LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	if (nCode == HC_ACTION && InGame())
	{
		if (!!!(lParam & INT_MIN)) //key is in a held state
		{
			if (UseKeyboardZoom)
			{
				if (wParam == 109) // Num -
				{
					do if (DZoom(DTWidth, DTHeight, DDif, 8))
						HandleBufferResize((SHORT)DTWidth, (SHORT)DTHeight);
					else break;
					while (ProgressiveResize && KeyPressed(109));
				}

				else if (wParam == 107) // Num +
				{
					do if (DZoom(DTWidth, DTHeight, DDif, -8))
						HandleBufferResize((SHORT)DTWidth, (SHORT)DTHeight);
					else break;
					while (ProgressiveResize && KeyPressed(107));
				}

				else if (wParam == VK_END) // End
				{
					ReNormalizeBuffers();
				}
			}

			if (AltEnter)
			{
				if (wParam == VK_RETURN)
				{
					if (!!(lParam & 0x20000000) && DDObj()) //Alt is pressed and DD is there
					{
						DWORD width, height;
						GetWndSize(WormsWnd(), width, height);

						if (width >= ScreenCX && height >= ScreenCY)
							DDObj()->SetDisplayMode(AeWidth, AeHeight, 0);
						else
						{
							AeWidth = width;
							AeHeight = height;
							DDObj()->SetDisplayMode(ScreenCX, ScreenCY, 0);
						}

						ReNormalizeBuffers();
					}
				}
			}
		}
	}
	return CallNextHookEx(kHook, nCode, wParam, lParam);
}

void InstallHooks()
{
	if (AllowResize || AllowZoom)
	{
		if (AllowResize)
		{
			if (!wHook) wHook = SetWindowsHookEx(WH_CALLWNDPROC, (HOOKPROC)CallWndProc, 0, GetCurrentThreadId());
			if (AltEnter && !kHook) kHook = SetWindowsHookEx(WH_KEYBOARD, (HOOKPROC)KeyboardProc, 0, GetCurrentThreadId());
		}
		if (AllowZoom)
		{
			if (UseTouchscreenZoom && !wHook) wHook = SetWindowsHookEx(WH_CALLWNDPROC, (HOOKPROC)CallWndProc, 0, GetCurrentThreadId());
			if (UseKeyboardZoom && !kHook) kHook = SetWindowsHookEx(WH_KEYBOARD, (HOOKPROC)KeyboardProc, 0, GetCurrentThreadId());
			if (UseMouseWheel   && !mHook) mHook = SetWindowsHookEx(WH_MOUSE, (HOOKPROC)MouseProc, 0, GetCurrentThreadId());
		}
	}
}

void UninstallHooks()
{
	if (wHook) UnhookWindowsHookEx(wHook);
	if (kHook) UnhookWindowsHookEx(kHook);
	if (mHook) UnhookWindowsHookEx(mHook);
}
