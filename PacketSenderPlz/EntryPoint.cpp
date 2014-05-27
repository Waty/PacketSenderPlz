#include <thread>
#include <iostream>
#include <Windows.h>
#include "MsPacket.h"
#include <nana\gui\wvl.hpp>
#include <nana\gui\place.hpp>
#include <nana\threads\pool.hpp>
#include <nana\gui\widgets\button.hpp>
#include <nana\gui\widgets\textbox.hpp>
#include <nana\gui\widgets\progress.hpp>

using namespace nana::gui;
using namespace nana::threads;

int nSendCount = 0, nLineDelay = 0, nLoopDelay = 0;
HINSTANCE hInstance = NULL;

void Log(std::string message)
{
#ifdef _DEBUG
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

void parseTextboxToInt(const textbox& sender, const eventinfo& info, int& value)
{
	nana::string str;
	int number = 0;
	if (sender.getline(0, str))
	{
		try { number = std::stoi(str); }
		catch (const std::invalid_argument&) {}
	}
	value = number;
}

void SendAllPackets(const textbox& tb)
{
	auto SendAllLines = [&]
	{
		static nana::string str;
		static MsPacket p;

		for (int line = 0; tb.getline(line, str); line++)
		{
			if (p.Parse(static_cast<std::string>(nana::charset(str)))) {
				p.Send();
			}
			std::this_thread::sleep_for(std::chrono::milliseconds(nLineDelay));
		}
	};

	if (nSendCount > 0)
	{
		for (int nSentCount = 0; nSentCount < nSendCount; nSentCount++)
		{
			SendAllLines();
			std::this_thread::sleep_for(std::chrono::milliseconds(nLoopDelay));
		}
	}
	else
	{
		while (nSendCount == 0)//TODO: Implement a button to break free from this loop!
		{
			SendAllLines();
			std::this_thread::sleep_for(std::chrono::milliseconds(nLoopDelay));
		}
	}
}

//Checks if the packet is valid, if it is valid, it saves the packet(s) to a file and enables the send button
void tbPacket_KeyUp(textbox& tbPackets, button& bSend)
{
	static MsPacket p;
	static nana::string str;
	bool bIsValidPacket = true;
	for (int i = 0; tbPackets.getline(i, str); i++)
	{
		if (!p.Parse(static_cast<std::string>(nana::charset(str))))
		{
			bIsValidPacket = false;
		}
	}

	bSend.enabled(bIsValidPacket);
	if (bIsValidPacket) tbPackets.store(L"C:\\Test");
}

void PacketSenderCloses(const eventinfo& info)
{
	msgbox box(info.window, L"Close MS?", msgbox::button_t::yes_no_cancel);
	box.icon(msgbox::icon_t::icon_question) << L"Do you want to close MapleStory too?";
	switch (box.show())
	{
	case msgbox::pick_yes:
		TerminateProcess(GetCurrentProcess(), EXIT_SUCCESS);
		break;

	case msgbox::pick_no:
		FreeLibraryAndExitThread(hInstance, EXIT_SUCCESS);
		break;

	case msgbox::pick_cancel:
		info.unload.cancel = true;
		break;
	}
}

DWORD WINAPI Start(LPVOID lpInstance)
{
	hInstance = reinterpret_cast<HINSTANCE>(lpInstance);

	form fm;
	fm.caption(L"Waty's PacketSenderPlz");
	fm.make_event<events::unload>(PacketSenderCloses);

	textbox tbPackets(fm), tbSendCount(fm), tbLineDelay(fm), tbLoopDelay(fm);
	//This gives them a tip_string, disables multiline and adds automatical parsing of the numbers entered in them
	tbSendCount.tip_string(L"SendCount").multi_lines(false).make_event<events::key_up>(std::bind(parseTextboxToInt, std::ref(tbSendCount), std::placeholders::_1, std::ref(nSendCount)));
	tbLineDelay.tip_string(L"LineDelay").multi_lines(false).make_event<events::key_up>(std::bind(parseTextboxToInt, std::ref(tbLineDelay), std::placeholders::_1, std::ref(nLineDelay)));
	tbLoopDelay.tip_string(L"LoopDelay").multi_lines(false).make_event<events::key_up>(std::bind(parseTextboxToInt, std::ref(tbLoopDelay), std::placeholders::_1, std::ref(nLoopDelay)));

	pool thrpool;
	button bSend(fm, L"Send");
	bSend.make_event<events::click>([&]	{ thrpool.push(std::bind(SendAllPackets, std::cref(tbPackets))); });

	//Init the textbox
	tbPackets.load(L"C:\\Test");
	tbPackets.tip_string(L"Type your scripts/packets here!");
	tbPackets.make_event<events::key_up>(std::bind(tbPacket_KeyUp, std::ref(tbPackets), std::ref(bSend)));

	//Fix the layout of everything
	place plc(fm);
	plc.div("<vertical margin=7 <textbox> <weight=7> <weight=25 controls > >");
	plc.field("textbox") << tbPackets;
	plc.field("controls") << tbSendCount << 7 << tbLineDelay << 7 << tbLoopDelay << 7 << bSend;
	plc.collocate();

	fm.show();
	exec();

	auto result = FreeConsole();

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
