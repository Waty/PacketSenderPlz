#include "stdafx.h"
#include "MsPacket.h"

using namespace nana::threads;
using namespace nana;

HINSTANCE hInstance;
int nSendCount = 0, nLineDelay = 0, nLoopDelay = 0;
std::atomic<bool> bIsCancelled;

void Log(const std::string& message)
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

void OnUnload(const arg_unload& info)
{
	msgbox box(info.window_handle, L"Close MS?", msgbox::button_t::yes_no_cancel);
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
		info.cancel = true;
		break;
	}
}

void SendAllLines(const std::vector<string>& lines)
{
	MsPacket p;

	for (string str : lines)
	{
		if (bIsCancelled) break;
		if (!p.Parse(static_cast<std::string>(nana::charset(str))) || !p.Send())
		{
			bIsCancelled = true;
			Log(p.GetError());
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(nLineDelay));
	}
}

void SendPackets(const std::vector<string> &lines, progress & progressbar)
{
	Log("Sending " + std::to_string(lines.size()) + " packets " + std::to_string(nSendCount) + " times");

	bIsCancelled = false;
	if (nSendCount > 0)
	{
		progressbar.unknown(false);
		progressbar.amount(nSendCount);
		for (int nTimesSent = 0; nTimesSent < nSendCount; nTimesSent++)
		{
			if (bIsCancelled) break;
			progressbar.value(nTimesSent);
			SendAllLines(lines);
			std::this_thread::sleep_for(std::chrono::milliseconds(nLoopDelay));
		}
	}
	else
	{
		progressbar.unknown(true);
		while (!bIsCancelled)
		{
			progressbar.inc();
			SendAllLines(lines);
			std::this_thread::sleep_for(std::chrono::milliseconds(nLoopDelay));
		}
	}
}

void WhitelistNumbers(const nana::arg_keyboard& info)
{
	info.ignore = (info.key >= 'a' && info.key <= 'z') || (info.key >= 'A' && info.key <= 'Z');
}

DWORD WINAPI Start(LPVOID lpInstance)
{
	nana::form fm;
	fm.caption(L"Waty's PacketSenderPlz v2.4");
	fm.events().unload(OnUnload);

	//Initialize all controls:
	place plc(fm);
	progress progressbar(fm);
	textbox tbPackets(fm), tbSendCount(fm), tbLineDelay(fm), tbLoopDelay(fm);
	button bSend(fm, L"Send"), bCancel(fm, L"Cancel");
	pool thrpool(1);

	API::eat_tabstop(tbPackets, false);

	tbSendCount.tip_string(L"SendCount").multi_lines(false);
	tbSendCount.tooltip(L"Specifies how many times this packet will be sent.");
	tbSendCount.events().key_char(WhitelistNumbers);
	API::eat_tabstop(tbSendCount, false);

	tbLineDelay.tip_string(L"LineDelay").multi_lines(false);
	tbLineDelay.tooltip(L"Specifies the delay in milliseconds that the sender will wait before sending the next line");
	tbLineDelay.events().key_char(WhitelistNumbers);
	API::eat_tabstop(tbLineDelay, false);

	tbLoopDelay.tip_string(L"LoopDelay").multi_lines(false);
	tbLoopDelay.tooltip(L"The delay in milliseconds that the sender will wait before restarting the loop");
	tbLoopDelay.events().key_char(WhitelistNumbers);
	API::eat_tabstop(tbLoopDelay, false);

	auto showProgress = [&]
	{
		plc.erase(tbSendCount);
		plc.erase(tbLineDelay);
		plc.erase(tbLoopDelay);
		plc.erase(bSend);
		bSend.hide();
		tbSendCount.hide();
		tbLineDelay.hide();
		tbLoopDelay.hide();
		plc.field("controls") << progressbar << bCancel;
		plc.collocate();
	};

	auto hideProgress = [&]
	{
		plc.erase(progressbar);
		plc.erase(bCancel);
		progressbar.hide();
		bCancel.hide();
		plc.field("controls") << tbSendCount << tbLineDelay << tbLoopDelay << bSend;
		plc.collocate();
	};

	bSend.events().click([&] {
		thrpool.push([&]{
			showProgress();

			nSendCount = tbSendCount.to_int();
			nLineDelay = tbLineDelay.to_int();
			nLoopDelay = tbLoopDelay.to_int();

			std::vector<string> lines;
			string str;
			for (size_t i = 0; tbPackets.getline(i, str); i++) lines.push_back(str);
			SendPackets(lines, progressbar);

			hideProgress();
		});
	});
	bCancel.events().click([] { bIsCancelled = true; });

	//place the widgets in the correct fields
	plc.div("<vertical margin=7 <packets> <weight=7> <weight=25 <controls gap=7> > >");
	plc.field("packets") << tbPackets;
	hideProgress();

	fm.show();
	exec();

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
