
#pragma once

typedef void(*PFVOID)();

extern bool Cavern;
extern BYTE Version;
extern CHAR Config[MAX_PATH], LandFile[MAX_PATH];

extern SHORT SWidth, SHeight, GlobalEatLimit, TargetWidth, TargetHeight;
extern SHORT ScreenX, ScreenY;
extern BOOL OfflineCavernFloodFix;
extern PFVOID RenderGame;

BYTE CheckVersion();
BOOL CavernCheck();
void GetTargetScreenSize(SHORT nWidth, SHORT nHeight);
void GetAddresses();
void UnprotectAddresses();
void PatchMem(SHORT nWidth, SHORT nHeight);
