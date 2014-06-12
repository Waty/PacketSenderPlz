#pragma once
#include <atomic>
#include <memory>
#include <nana\gui\wvl.hpp>
#include <nana\gui\timer.hpp>
#include <nana\gui\place.hpp>
#include <nana\gui\widgets\button.hpp>
#include <nana\gui\widgets\textbox.hpp>
#include <nana\gui\widgets\progress.hpp>
#include <nana\threads\pool.hpp>

using namespace nana::gui;
using namespace nana::threads;

class MainForm : form
{
	std::atomic<bool> bIsCancelled;
	int nSendCount = 0, nLineDelay = 0, nLoopDelay = 0;
	std::shared_ptr<place> plc;
	std::shared_ptr<progress> progressbar;
	std::shared_ptr<textbox> tbPackets;

	void SendPackets();
	void SendAllLines();
	void OnUnload(const eventinfo& info);
	void ParseTextbox(const textbox& sender, const eventinfo& info, int& result);

public:
	enum class DisplayMode { Idle, Busy };
	MainForm();

	void SetDisplayMode(DisplayMode mode);
};
