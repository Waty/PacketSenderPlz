#include <Windows.h>
#include <string>
#include <msclr\marshal_cppstd.h>
#include "MainForm.h"
#include "MsPacket.h"

using namespace PacketSenderPlz;
using namespace System::Threading::Tasks;

HMODULE hModule = NULL;
std::vector<MsPacket*> packets;

DWORD WINAPI InitializeClr(LPVOID lpInstance)
{
	hModule = reinterpret_cast<HINSTANCE>(lpInstance);
	Application::EnableVisualStyles();
	Application::SetCompatibleTextRenderingDefault(false);
	Application::Run(gcnew MainForm());
	Application::Exit();
	return 0;
};

void Log(std::string message)
{
#ifdef _DEBUG
	HWND handle = GetConsoleWindow();
	if (handle == NULL) AllocConsole();
	else ShowWindow(handle, SW_SHOW);
	Console::WriteLine(gcnew String(message.c_str()));
#else
	MessageBox::Show(gcnew String(message.c_str()), "Error!");
#endif
}

Void MainForm::StartSending(Object^ sender, EventArgs^ e)
{
	if (tbPackets->Lines->Length == 0)
	{
		Log("The packets TextBox was empty!");
		return;
	}

	if (sender == bSend) Mode = PacketMode::Send;
	else Mode = PacketMode::Recv;

	//Mem managment
	for (auto p : packets) delete p;
	packets.clear();
	for each (auto line in tbPackets->Lines)
	{
		std::string packet = msclr::interop::marshal_as<std::string>(line);
		MsPacket* p = new MsPacket();

		if (p->Parse(packet)) packets.push_back(p);
		else
		{
			Log("Tried parsing '" + packet + "' but failed:\n" + p->GetError());
			return;
		}
	}

	pProgressCancel->Visible = true;
	tbPackets->Enabled = false;
	bwSender->RunWorkerAsync();
}

Void MainForm::bCancel_Click(Object^ sender, EventArgs^ e)
{
	bwSender->CancelAsync();
}

Void MainForm::SendAllPackets()
{
	size_t size = packets.size();
	for (auto p : packets)
	{
		if (bwSender->CancellationPending) return;
		if (!p->Send())
		{
			Log("Failed sending '" + p->ToString() + "':\n" + p->GetError());
			bwSender->CancelAsync();
		}
		Sleep(LineDelay);
	}
}

Void MainForm::bwSender_DoWork(Object^ sender, DoWorkEventArgs^ e)
{
	if (SendCount > 0)
	{
		for (int nSentCount = 0; nSentCount < SendCount; nSentCount++)
		{
			if (bwSender->CancellationPending) break;
			bwSender->ReportProgress(nSentCount == 0 ? 0 : 100l * nSentCount / SendCount);
			SendAllPackets();
			Sleep(LoopDelay);
		}
	}
	else
	{
		while (!bwSender->CancellationPending)
		{
			bwSender->ReportProgress(100);
			SendAllPackets();
			Sleep(LoopDelay);
		}
	}
}

Void MainForm::bwSender_ProgressChanged(Object^ sender, ProgressChangedEventArgs^ e)
{
	pbProgress->Maximum = 100;
	pbProgress->Value = e->ProgressPercentage;
}

Void MainForm::bwSender_RunWorkerCompleted(Object^ sender, RunWorkerCompletedEventArgs^ e)
{
	pProgressCancel->Visible = false;
	tbPackets->Enabled = true;
}

Void MainForm::MainForm_FormClosing(Object^ sender, FormClosingEventArgs^ e)
{
	switch (MessageBox::Show("Do you want to close MapleStory too?", "Close MS too?", MessageBoxButtons::YesNoCancel, MessageBoxIcon::Question))
	{
	case ::DialogResult::Yes:
		TerminateProcess(GetCurrentProcess(), 0);
		break;

	case ::DialogResult::No:
		FreeLibraryAndExitThread(hModule, 0);
		break;

	case ::DialogResult::Cancel:
		e->Cancel = true;
		break;
	}
}
