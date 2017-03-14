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
	msgbox box(info.window_handle, "Close MS?", msgbox::button_t::yes_no_cancel);
	box.icon(msgbox::icon_t::icon_question) << "Do you want to close MapleStory too?";
	switch (box.show())
	{
	case msgbox::pick_yes:
		TerminateProcess(GetCurrentProcess(), EXIT_SUCCESS);
		break;

	case msgbox::pick_no:
		FreeLibraryAndExitThread(instance_handle, EXIT_SUCCESS);

	default:
	case msgbox::pick_cancel:
		info.cancel = true;
		break;
	}
}

void send_all_lines(const std::vector<std::string>& lines)
{
	packet p;

	for (auto str : lines)
	{
		if (cancelled) break;
		if (!p.parse(str) || !p.send())
		{
			cancelled = true;
			log(p.get_error());
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(line_delay));
	}
}

void send_packets(const std::vector<std::string>& lines, progress& progressbar)
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
	fm.caption(u8"Waty's PacketSenderPlz v2.4");
	fm.events().unload(on_unload);

	//Initialize all controls:
	place plc(fm);
	progress progressbar(fm);
	textbox tbPackets(fm), tbSendCount(fm), tbLineDelay(fm), tbLoopDelay(fm);
	button bSend(fm, "Send"), bCancel(fm, "Cancel");
	pool thrpool(1);

	tbPackets.set_accept([](wchar_t c) { return iswxdigit(c) || iswcntrl(c) || c == '*' || c == ' '; });
	API::eat_tabstop(tbPackets, false);

	auto filter = [](wchar_t c) { return iswdigit(c) || iswcntrl(c); };
	tbSendCount.tip_string("SendCount").multi_lines(false);
	tbSendCount.tooltip("Specifies how many times this packet will be sent.");
	tbSendCount.set_accept(filter);
	API::eat_tabstop(tbSendCount, false);

	tbLineDelay.tip_string("LineDelay").multi_lines(false);
	tbLineDelay.tooltip("Specifies the delay in milliseconds that the sender will wait before sending the next line");
	tbLineDelay.set_accept(filter);
	API::eat_tabstop(tbLineDelay, false);

	tbLoopDelay.tip_string("LoopDelay").multi_lines(false);
	tbLoopDelay.tooltip("The delay in milliseconds that the sender will wait before restarting the loop");
	tbLoopDelay.set_accept(filter);
	API::eat_tabstop(tbLoopDelay, false);

	auto show_progress = [&](bool show)
	{
		plc.field_display("controls", !show);
		plc.field_display("progress", show);
		plc.collocate();
	};

	bSend.events().click([&]
	{
		thrpool.push([&]
		{
			show_progress(true);

			send_count = tbSendCount.to_int();
			line_delay = tbLineDelay.to_int();
			loop_delay = tbLoopDelay.to_int();

			std::vector<std::string> lines;
			std::string str;
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
