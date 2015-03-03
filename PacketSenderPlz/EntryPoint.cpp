#include "stdafx.h"
#include "packet.h"

using namespace nana;
using namespace threads;

HINSTANCE instance_handle;
int send_count = 0, line_delay = 0, loop_delay = 0;
std::atomic<bool> cancelled;

void log(const std::string& message)
{
#ifdef _DEBUG
	HWND handle = GetConsoleWindow();
	if (handle == nullptr)
	{
		AllocConsole();
		FILE* stream;
		freopen_s(&stream, "CONOUT$", "w", stdout);
		freopen_s(&stream, "CONIN$", "r", stdin);
	}
	else ShowWindow(handle, SW_SHOW);
	std::cout << message << '\n';
#else
	MessageBoxA(nullptr, message.c_str(), nullptr, 0);
#endif
}

void on_unload(const arg_unload& info)
{
	msgbox box(info.window_handle, L"Close MS?", msgbox::button_t::yes_no_cancel);
	box.icon(msgbox::icon_t::icon_question) << L"Do you want to close MapleStory too?";
	switch (box.show())
	{
	case msgbox::pick_yes:
		TerminateProcess(GetCurrentProcess(), EXIT_SUCCESS);
		break;

	case msgbox::pick_no:
		FreeLibraryAndExitThread(instance_handle, EXIT_SUCCESS);
		break;

	default:
	case msgbox::pick_cancel:
		info.cancel = true;
		break;
	}
}

void send_all_lines(const std::vector<string>& lines)
{
	packet p;

	for (auto str : lines)
	{
		if (cancelled) break;
		if (!p.parse(static_cast<std::string>(charset(str))) || !p.send())
		{
			cancelled = true;
			log(p.get_error());
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(line_delay));
	}
}

void send_packets(const std::vector<string>& lines, progress& progressbar)
{
#ifdef _DEBUG
	log("Sending " + std::to_string(lines.size()) + " packets " + std::to_string(send_count) + " times");
#endif

	cancelled = false;
	if (send_count > 0)
	{
		progressbar.unknown(false);
		progressbar.amount(send_count);
		for (int times_sent = 0; times_sent < send_count; times_sent++)
		{
			if (cancelled) break;
			progressbar.value(times_sent);
			send_all_lines(lines);
			std::this_thread::sleep_for(std::chrono::milliseconds(loop_delay));
		}
	}
	else
	{
		progressbar.unknown(true);
		while (!cancelled)
		{
			progressbar.inc();
			send_all_lines(lines);
			std::this_thread::sleep_for(std::chrono::milliseconds(loop_delay));
		}
	}
}

DWORD WINAPI start(LPVOID /*lpInstance*/)
{
	form fm;
	fm.caption(L"Waty's PacketSenderPlz v2.4");
	fm.events().unload(on_unload);

	//Initialize all controls:
	place plc(fm);
	progress progressbar(fm);
	textbox tbPackets(fm), tbSendCount(fm), tbLineDelay(fm), tbLoopDelay(fm);
	button bSend(fm, L"Send"), bCancel(fm, L"Cancel");
	pool thrpool(1);

	tbPackets.set_accept([](char_t c) { return iswxdigit(c) || iswcntrl(c) || c == '*' || c == ' '; });
	API::eat_tabstop(tbPackets, false);

	auto filter = [](char_t c) { return iswdigit(c) || iswcntrl(c); };
	tbSendCount.tip_string(L"SendCount").multi_lines(false);
	tbSendCount.tooltip(L"Specifies how many times this packet will be sent.");
	tbSendCount.set_accept(filter);
	API::eat_tabstop(tbSendCount, false);

	tbLineDelay.tip_string(L"LineDelay").multi_lines(false);
	tbLineDelay.tooltip(L"Specifies the delay in milliseconds that the sender will wait before sending the next line");
	tbLineDelay.set_accept(filter);
	API::eat_tabstop(tbLineDelay, false);

	tbLoopDelay.tip_string(L"LoopDelay").multi_lines(false);
	tbLoopDelay.tooltip(L"The delay in milliseconds that the sender will wait before restarting the loop");
	tbLoopDelay.set_accept(filter);
	API::eat_tabstop(tbLoopDelay, false);

	auto show_progress = [&](bool show)
	{
		plc.field_display("controls", !show);
		plc.field_display("progress", show);
		plc.collocate();
	};

	bSend.events().click([&] {
		thrpool.push([&]{
			show_progress(true);

			send_count = tbSendCount.to_int();
			line_delay = tbLineDelay.to_int();
			loop_delay = tbLoopDelay.to_int();

			std::vector<string> lines;
			string str;
			for (size_t i = 0; tbPackets.getline(i, str); i++) lines.push_back(str);
			send_packets(lines, progressbar);

			show_progress(false);
		});
	});
	bCancel.events().click([] { cancelled = true; });

	//place the widgets in the correct fields
	plc.div("<vertical margin=7 <packets> <weight=7> <weight=25 <controls gap=7> <progress gap=7> > >");
	plc.field("packets") << tbPackets;
	plc.field("progress") << progressbar << bCancel;
	plc.field("controls") << tbSendCount << tbLineDelay << tbLoopDelay << bSend;
	show_progress(false);

	fm.show();
	exec();

	return EXIT_SUCCESS;
}

int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, LPSTR /*lpCmdLine*/, int /*nCmdShow*/)
{
	start(hInstance);
}

BOOL WINAPI DllMain(HMODULE module, DWORD fdwReason, LPVOID /*lpvReserved*/)
{
	if (fdwReason == DLL_PROCESS_ATTACH)
		CreateThread(nullptr, NULL, start, module, NULL, nullptr);

	return TRUE;
}
