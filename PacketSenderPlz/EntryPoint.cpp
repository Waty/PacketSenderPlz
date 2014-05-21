#include <Windows.h>

extern DWORD WINAPI InitializeClr(LPVOID lpInstance);

int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	InitializeClr(hInstance);
}

BOOL WINAPI DllMain(HMODULE hModule, DWORD fdwReason, LPVOID lpvReserved)
{
	if (fdwReason == DLL_PROCESS_ATTACH)
		CreateThread(NULL, NULL, InitializeClr, hModule, NULL, NULL);

	return TRUE;
}
