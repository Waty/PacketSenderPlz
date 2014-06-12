#include <thread>
#include <atomic>
#include <iostream>
#include <Windows.h>
#include "MsPacket.h"
#include <nana\gui\wvl.hpp>
#include <nana\gui\place.hpp>
#include <nana\threads\pool.hpp>
#include <nana\gui\widgets\button.hpp>
#include <nana\gui\widgets\textbox.hpp>
#include <nana\gui\widgets\progress.hpp>
#include "MainForm.h"

using namespace nana::gui;
using namespace nana::threads;

extern HINSTANCE hInstance;

void Log(const std::string& message)
{
#ifndef _DEBUG
	HWND handle = GetConsoleWindow();
	if (handle == NULL)
	{
		AllocConsole();
		FILE* stream;
		freopen_s(&stream, "CONOUT$", "w", stdout);
		freopen_s(&stream, "CONIN$", "r", stdin);
	}
	else ShowWindow(handle, SW_SHOW);
	std::cout << message << '\n';
#else
	MessageBoxA(0, message.c_str(), 0, 0);
#endif
}

DWORD WINAPI Start(LPVOID lpInstance)
{
	MainForm mf;

	return EXIT_SUCCESS;
}

int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	Start(hInstance);
}

BOOL WINAPI DllMain(HMODULE hModule, DWORD fdwReason, LPVOID lpvReserved)
{
	if (fdwReason == DLL_PROCESS_ATTACH)
		CreateThread(NULL, NULL, Start, hModule, NULL, NULL);

	return TRUE;
}
