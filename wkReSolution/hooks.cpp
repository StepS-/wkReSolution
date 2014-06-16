
#include <windows.h>
#include "hooks.h"
#include "w2res.h"

HHOOK wHook, kHook, mHook;
BOOL ModifiedSurfaces;

SHORT TWidth, THeight, LastWidth, LastHeight;
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

	SHORT width, height;
	GetW2WndSize(width, height);

	if (HandleBufferResize(width, height))
	{
		DTWidth = width;
		DTHeight = height;
		DDif = DTHeight / DTWidth;
		result = 1;
	}

	return result;
}

BOOL HandleBufferResize(SHORT nWidth, SHORT nHeight, bool bRedraw)
{
	BOOL result = 0;

	if (DDObj && nWidth > 0 && nHeight > 0)
	{
		TWidth = nWidth;
		THeight = nHeight;
		ModifiedSurfaces = 0;
		if (LastWidth != TWidth || LastHeight != THeight)
		if (SUCCEEDED(DDObj->EnumSurfaces(DDENUMSURFACES_DOESEXIST | DDENUMSURFACES_ALL, NULL, NULL, EnumResize)))
		{
			LastWidth = TWidth;
			LastHeight = THeight;
			PatchMem(TWidth, THeight, true);
			W2DS->RenderWidth = TargetWidth;
			W2DS->RenderHeight = TargetHeight;
			if (ModifiedSurfaces && (ProgressiveResize || bRedraw))
				RenderGame(); //Experimental: rerender the scene right after resizing
			result = 1;
		}
	}

	return result;
}

//HACK
HRESULT WINAPI EnumResize(LPDIRECTDRAWSURFACE pSurface, LPDDSURFACEDESC lpSurfaceDesc, LPVOID lpContext)
{
	BOOL bRequiredSurface  = (!!!(lpSurfaceDesc->dwFlags & DDSD_CKSRCBLT) && lpSurfaceDesc->dwWidth == LastWidth && lpSurfaceDesc->dwHeight == LastHeight);
	BOOL bPrimary          = ( !!(lpSurfaceDesc->ddsCaps.dwCaps & DDSCAPS_PRIMARYSURFACE));

	if (bRequiredSurface && !bPrimary)
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

			*lpDataHeight = THeight;
			*lpInfoHeight = THeight;

			//cleanup
			memset((PVOID)(*lpSurfMemAddr), 0, dwNewMemSize);

			ModifiedSurfaces++;
			LocalUnlock(MemAlloc);
		}
	}
	return DDENUMRET_OK;
}

LRESULT CALLBACK CallWndProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	if (nCode == HC_ACTION)
	{
		CWPSTRUCT* pwp = (CWPSTRUCT*)lParam;
		if (pwp->message == WM_WINDOWPOSCHANGED)
		{
			if (pwp->hwnd == W2Wnd)
			{
				LPWINDOWPOS lwp = (LPWINDOWPOS)(pwp->lParam);
				if (!!!(lwp->flags & SWP_NOSIZE) && !!!(lwp->flags & SWP_NOCOPYBITS) && !!!(lwp->flags & SWP_NOSENDCHANGING))
					ReNormalizeBuffers();
			}
		}
	}

	return CallNextHookEx(wHook, nCode, wParam, lParam);
}

LRESULT CALLBACK MouseProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	if (nCode == HC_ACTION)
	{
		if (wParam == WM_MOUSEWHEEL)
		{
			LPMOUSEHOOKSTRUCTEX lpWheelInf = (LPMOUSEHOOKSTRUCTEX)lParam;
			SHORT sDelta = HIWORD(lpWheelInf->mouseData);
			if (sDelta != 0)
			{
				if (DZoom(DTWidth, DTHeight, DDif, -sDelta))
					HandleBufferResize((SHORT)DTWidth, (SHORT)DTHeight);
			}
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
	PBYTE kstate = 0;

	if (HC_ACTION == nCode)
	{
		if (!!!(lParam & INT_MIN)) //key is in a held state
		{
			if (UseKeyboardZoom)
			{
				if (wParam == 109) // Num -
				{
					do if (DZoom(DTWidth, DTHeight, DDif, 8))
						HandleBufferResize((SHORT)DTWidth, (SHORT)DTHeight, true);
					else break;
					while (KeyPressed(109));
				}

				else if (wParam == 107) // Num +
				{
					do if (DZoom(DTWidth, DTHeight, DDif, -8))
						HandleBufferResize((SHORT)DTWidth, (SHORT)DTHeight, true);
					else break;
					while (KeyPressed(107));
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
					if (!!(lParam & 0x20000000) && DDObj) //Alt is pressed and DD is there
					{
						SHORT width, height;
						GetW2WndSize(width, height);

						if (width >= ScreenCX && height >= ScreenCY)
							DDObj->SetDisplayMode(AeWidth, AeHeight, 0);
						else
						{
							AeWidth = width;
							AeHeight = height;
							DDObj->SetDisplayMode(ScreenCX, ScreenCY, 0);
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
