#include <thread>
#include <Windows.h>
#include "MainForm.h"
#include "MsPacket.h"

#include <nana\gui\place.hpp>

extern void Log(const std::string& message);
HINSTANCE hInstance = NULL;

MainForm::MainForm() : form(API::make_center(450, 150))
{
	this->caption(L"Waty's PacketSenderPlz v2.4");
	this->make_event<events::unload>(*this, &MainForm::OnUnload);

	//Initialize all controls:
	tbPackets.reset(new textbox(*this));

	textbox tbSendCount(*this), tbLineDelay(*this), tbLoopDelay(*this);
	button bSend(*this, L"Send"), bCancel(*this, L"Cancel");
	pool thrpool(1);

	plc.reset(new place(*this));
	progressbar.reset(new progress(*this));

	tbPackets->load(L"C:\\packets.txt");
	tbPackets->tip_string(L"Please input the packets in this textbox")
		.make_event<events::key_char>([this] { tbPackets->store(L"C:\\packets.txt"); });

	tbSendCount.tip_string(L"SendCount").multi_lines(false).tooltip(L"Specifies how many times this packet will be sent.")
		.make_event<events::key_up>(std::bind(&MainForm::ParseTextbox, this, std::cref(tbSendCount), std::placeholders::_1, std::ref(nSendCount)));

	tbLineDelay.tip_string(L"LineDelay").multi_lines(false).tooltip(L"Specifies the delay in milliseconds that the sender will wait before sending the next line")
		.make_event<events::key_up>(std::bind(&MainForm::ParseTextbox, this, std::cref(tbLineDelay), std::placeholders::_1, std::ref(nLineDelay)));

	tbLoopDelay.tip_string(L"LoopDelay").multi_lines(false).tooltip(L"The delay in milliseconds that the sender will wait before restarting the loop")
		.make_event<events::key_up>(std::bind(&MainForm::ParseTextbox, this, std::cref(tbLoopDelay), std::placeholders::_1, std::ref(nLoopDelay)));

	bSend.make_event<events::click>([&] { thrpool.push(std::bind(&MainForm::SendPackets, this)); });
	bCancel.make_event<events::click>([this] { bIsCancelled = true; });


	//place the widgets in the correct fields
	plc->field("packets") << *tbPackets;
	plc->field("controls") << tbSendCount << 7 << tbLineDelay << 7 << tbLoopDelay << 7 << bSend;
	plc->field("progress") << *progressbar << 7 << plc->percent(bCancel, 25);
	SetDisplayMode(DisplayMode::Idle);

	show();
	exec();
}

void MainForm::SetDisplayMode(DisplayMode mode)
{
	if (mode == DisplayMode::Idle) plc->div("<vertical margin=7 <packets> <weight=7> <weight=25 <controls> > >");
	if (mode == DisplayMode::Busy) plc->div("<vertical margin=7 <packets> <weight=7> <weight=25 <progress> > >");

	plc->collocate();
}
void MainForm::SendAllLines()
{
	static MsPacket p;

	nana::string str;
	for (int i = 0; tbPackets->getline(i, str); i++)
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

void MainForm::SendPackets()
{
	bIsCancelled = false;
	SetDisplayMode(DisplayMode::Busy);
	if (nSendCount > 0)
	{
		progressbar->unknown(false);
		progressbar->amount(nSendCount);
		for (int nTimesSent = 0; nTimesSent < nSendCount; nTimesSent++)
		{
			if (bIsCancelled) break;
			progressbar->value(nTimesSent);
			SendAllLines();
			std::this_thread::sleep_for(std::chrono::milliseconds(nLoopDelay));
		}
	}
	else
	{
		progressbar->unknown(true);
		while (!bIsCancelled)
		{
			progressbar->inc();
			SendAllLines();
			std::this_thread::sleep_for(std::chrono::milliseconds(nLoopDelay));
		}
	}
	SetDisplayMode(DisplayMode::Idle);
}

void MainForm::ParseTextbox(const textbox& sender, const eventinfo& info, int& result)
{
	info.keyboard.ignore = (info.keyboard.key >= 'a' && info.keyboard.key <= 'z') || (info.keyboard.key >= 'A' && info.keyboard.key <= 'Z');

	nana::string str;
	if (sender.getline(0, str))
	{
		try { result = std::stoi(str); }
		catch (const std::invalid_argument&) {}
	}
}

void MainForm::OnUnload(const eventinfo& info)
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
