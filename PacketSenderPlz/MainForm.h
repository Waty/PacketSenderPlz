#pragma once
using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;

namespace PacketSenderPlz {
	public enum PacketMode { Send, Recv };
	/// <summary>
	/// Summary for MainForm
	/// </summary>
	public ref class MainForm : public System::Windows::Forms::Form
	{
	public:
		MainForm(void)
		{
			InitializeComponent();
		}

	protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~MainForm()
		{
			if (components)
			{
				delete components;
			}
		}
	private:
		PacketMode Mode;
		property int SendCount
		{
			int get() { return Decimal::ToInt32(nudSpamCount->Value); }
			void set(int value) { pbProgress->Maximum = value; nudSpamCount->Value = value; }
		}
		property int LineDelay
		{
			int get() { return Decimal::ToInt32(nudLineDelay->Value); }
			void set(int value) { nudLineDelay->Value = value; };
		}
		property int LoopDelay
		{
			int get() { return Decimal::ToInt32(nudLoopDelay->Value); }
			void set(int value) { nudLoopDelay->Value = value; };
		}


		Panel^ pProgressCancel;
		Panel^ pSendRecvButtons;
		Button^ bSend;
		Button^ bRecv;
		Button^ bCancel;
		TextBox^ tbPackets;
		ProgressBar^ pbProgress;
		NumericUpDown^ nudSpamCount;
		NumericUpDown^ nudLineDelay;
		NumericUpDown^ nudLoopDelay;
		Windows::Forms::Label^ lThePackets;
		Windows::Forms::Label^ lOr;
		Windows::Forms::Label^ lSpamCount;
		Windows::Forms::Label^ lLineDelay;
		Windows::Forms::Label^ lLoopDelay;

		BackgroundWorker^  bwSender;


		/// <summary>
		/// Required designer variable.
		/// </summary>
		IContainer^ components;

#pragma region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		void InitializeComponent(void)
		{
			this->tbPackets = (gcnew System::Windows::Forms::TextBox());
			this->bSend = (gcnew System::Windows::Forms::Button());
			this->lOr = (gcnew System::Windows::Forms::Label());
			this->bRecv = (gcnew System::Windows::Forms::Button());
			this->lLineDelay = (gcnew System::Windows::Forms::Label());
			this->lSpamCount = (gcnew System::Windows::Forms::Label());
			this->lThePackets = (gcnew System::Windows::Forms::Label());
			this->pSendRecvButtons = (gcnew System::Windows::Forms::Panel());
			this->pProgressCancel = (gcnew System::Windows::Forms::Panel());
			this->bCancel = (gcnew System::Windows::Forms::Button());
			this->pbProgress = (gcnew System::Windows::Forms::ProgressBar());
			this->nudSpamCount = (gcnew System::Windows::Forms::NumericUpDown());
			this->nudLineDelay = (gcnew System::Windows::Forms::NumericUpDown());
			this->nudLoopDelay = (gcnew System::Windows::Forms::NumericUpDown());
			this->lLoopDelay = (gcnew System::Windows::Forms::Label());
			this->bwSender = (gcnew System::ComponentModel::BackgroundWorker());
			this->pSendRecvButtons->SuspendLayout();
			this->pProgressCancel->SuspendLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->nudSpamCount))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->nudLineDelay))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->nudLoopDelay))->BeginInit();
			this->SuspendLayout();
			// 
			// tbPackets
			// 
			this->tbPackets->AcceptsReturn = true;
			this->tbPackets->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom)
				| System::Windows::Forms::AnchorStyles::Left)
				| System::Windows::Forms::AnchorStyles::Right));
			this->tbPackets->CharacterCasing = System::Windows::Forms::CharacterCasing::Upper;
			this->tbPackets->Location = System::Drawing::Point(12, 12);
			this->tbPackets->Multiline = true;
			this->tbPackets->Name = L"tbPackets";
			this->tbPackets->ScrollBars = System::Windows::Forms::ScrollBars::Horizontal;
			this->tbPackets->Size = System::Drawing::Size(413, 74);
			this->tbPackets->TabIndex = 0;
			// 
			// bSend
			// 
			this->bSend->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom)
				| System::Windows::Forms::AnchorStyles::Left)
				| System::Windows::Forms::AnchorStyles::Right));
			this->bSend->Location = System::Drawing::Point(-1, 0);
			this->bSend->Name = L"bSend";
			this->bSend->Size = System::Drawing::Size(50, 36);
			this->bSend->TabIndex = 1;
			this->bSend->Text = L"Send";
			this->bSend->UseVisualStyleBackColor = true;
			this->bSend->Click += gcnew System::EventHandler(this, &MainForm::StartSending);
			// 
			// lOr
			// 
			this->lOr->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom)
				| System::Windows::Forms::AnchorStyles::Left)
				| System::Windows::Forms::AnchorStyles::Right));
			this->lOr->AutoSize = true;
			this->lOr->Location = System::Drawing::Point(55, 12);
			this->lOr->Name = L"lOr";
			this->lOr->Size = System::Drawing::Size(16, 13);
			this->lOr->TabIndex = 4;
			this->lOr->Text = L"or";
			this->lOr->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
			// 
			// bRecv
			// 
			this->bRecv->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom)
				| System::Windows::Forms::AnchorStyles::Left)
				| System::Windows::Forms::AnchorStyles::Right));
			this->bRecv->Enabled = false;
			this->bRecv->Location = System::Drawing::Point(77, 0);
			this->bRecv->Name = L"bRecv";
			this->bRecv->Size = System::Drawing::Size(50, 36);
			this->bRecv->TabIndex = 5;
			this->bRecv->Text = L"Recv";
			this->bRecv->UseVisualStyleBackColor = true;
			this->bRecv->Click += gcnew System::EventHandler(this, &MainForm::StartSending);
			// 
			// lLineDelay
			// 
			this->lLineDelay->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left));
			this->lLineDelay->AutoSize = true;
			this->lLineDelay->Location = System::Drawing::Point(289, 92);
			this->lLineDelay->Name = L"lLineDelay";
			this->lLineDelay->Size = System::Drawing::Size(60, 13);
			this->lLineDelay->TabIndex = 6;
			this->lLineDelay->Text = L"Line Delay:";
			this->lLineDelay->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
			// 
			// lSpamCount
			// 
			this->lSpamCount->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left));
			this->lSpamCount->AutoSize = true;
			this->lSpamCount->Location = System::Drawing::Point(218, 92);
			this->lSpamCount->Name = L"lSpamCount";
			this->lSpamCount->Size = System::Drawing::Size(68, 13);
			this->lSpamCount->TabIndex = 6;
			this->lSpamCount->Text = L"Spam Count:";
			this->lSpamCount->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
			// 
			// lThePackets
			// 
			this->lThePackets->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom)
				| System::Windows::Forms::AnchorStyles::Left)
				| System::Windows::Forms::AnchorStyles::Right));
			this->lThePackets->AutoSize = true;
			this->lThePackets->Location = System::Drawing::Point(133, 12);
			this->lThePackets->Name = L"lThePackets";
			this->lThePackets->Size = System::Drawing::Size(69, 13);
			this->lThePackets->TabIndex = 10;
			this->lThePackets->Text = L"the packet(s)";
			// 
			// pSendRecvButtons
			// 
			this->pSendRecvButtons->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left));
			this->pSendRecvButtons->Controls->Add(this->bSend);
			this->pSendRecvButtons->Controls->Add(this->bRecv);
			this->pSendRecvButtons->Controls->Add(this->lThePackets);
			this->pSendRecvButtons->Controls->Add(this->lOr);
			this->pSendRecvButtons->Location = System::Drawing::Point(12, 92);
			this->pSendRecvButtons->Name = L"pSendRecvButtons";
			this->pSendRecvButtons->Size = System::Drawing::Size(203, 36);
			this->pSendRecvButtons->TabIndex = 11;
			// 
			// pProgressCancel
			// 
			this->pProgressCancel->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left));
			this->pProgressCancel->Controls->Add(this->bCancel);
			this->pProgressCancel->Controls->Add(this->pbProgress);
			this->pProgressCancel->Location = System::Drawing::Point(12, 92);
			this->pProgressCancel->Name = L"pProgressCancel";
			this->pProgressCancel->Size = System::Drawing::Size(203, 36);
			this->pProgressCancel->TabIndex = 12;
			this->pProgressCancel->Visible = false;
			// 
			// bCancel
			// 
			this->bCancel->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom)
				| System::Windows::Forms::AnchorStyles::Left)
				| System::Windows::Forms::AnchorStyles::Right));
			this->bCancel->Location = System::Drawing::Point(155, 0);
			this->bCancel->Name = L"bCancel";
			this->bCancel->Size = System::Drawing::Size(48, 36);
			this->bCancel->TabIndex = 1;
			this->bCancel->Text = L"Cancel";
			this->bCancel->UseVisualStyleBackColor = true;
			this->bCancel->Click += gcnew System::EventHandler(this, &MainForm::bCancel_Click);
			// 
			// pbProgress
			// 
			this->pbProgress->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom)
				| System::Windows::Forms::AnchorStyles::Left)
				| System::Windows::Forms::AnchorStyles::Right));
			this->pbProgress->Location = System::Drawing::Point(0, 0);
			this->pbProgress->Name = L"pbProgress";
			this->pbProgress->Size = System::Drawing::Size(148, 36);
			this->pbProgress->Step = 1;
			this->pbProgress->TabIndex = 0;
			// 
			// nudSpamCount
			// 
			this->nudSpamCount->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left));
			this->nudSpamCount->Location = System::Drawing::Point(221, 108);
			this->nudSpamCount->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) { 2147483647, 0, 0, 0 });
			this->nudSpamCount->Name = L"nudSpamCount";
			this->nudSpamCount->Size = System::Drawing::Size(65, 20);
			this->nudSpamCount->TabIndex = 12;
			this->nudSpamCount->Value = System::Decimal(gcnew cli::array< System::Int32 >(4) { 1, 0, 0, 0 });
			// 
			// nudLineDelay
			// 
			this->nudLineDelay->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left));
			this->nudLineDelay->Location = System::Drawing::Point(292, 108);
			this->nudLineDelay->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) { 2147483647, 0, 0, 0 });
			this->nudLineDelay->Name = L"nudLineDelay";
			this->nudLineDelay->Size = System::Drawing::Size(65, 20);
			this->nudLineDelay->TabIndex = 13;
			this->nudLineDelay->Value = System::Decimal(gcnew cli::array< System::Int32 >(4) { 150, 0, 0, 0 });
			// 
			// nudLoopDelay
			// 
			this->nudLoopDelay->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left));
			this->nudLoopDelay->Location = System::Drawing::Point(363, 108);
			this->nudLoopDelay->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) { 2147483647, 0, 0, 0 });
			this->nudLoopDelay->Name = L"nudLoopDelay";
			this->nudLoopDelay->Size = System::Drawing::Size(60, 20);
			this->nudLoopDelay->TabIndex = 14;
			// 
			// lLoopDelay
			// 
			this->lLoopDelay->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left));
			this->lLoopDelay->AutoSize = true;
			this->lLoopDelay->Location = System::Drawing::Point(360, 92);
			this->lLoopDelay->Name = L"lLoopDelay";
			this->lLoopDelay->Size = System::Drawing::Size(64, 13);
			this->lLoopDelay->TabIndex = 15;
			this->lLoopDelay->Text = L"Loop Delay:";
			this->lLoopDelay->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
			// 
			// bwSender
			// 
			this->bwSender->WorkerReportsProgress = true;
			this->bwSender->WorkerSupportsCancellation = true;
			this->bwSender->DoWork += gcnew System::ComponentModel::DoWorkEventHandler(this, &MainForm::bwSender_DoWork);
			this->bwSender->ProgressChanged += gcnew System::ComponentModel::ProgressChangedEventHandler(this, &MainForm::bwSender_ProgressChanged);
			this->bwSender->RunWorkerCompleted += gcnew System::ComponentModel::RunWorkerCompletedEventHandler(this, &MainForm::bwSender_RunWorkerCompleted);
			// 
			// MainForm
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(441, 140);
			this->Controls->Add(this->lLoopDelay);
			this->Controls->Add(this->nudLoopDelay);
			this->Controls->Add(this->nudLineDelay);
			this->Controls->Add(this->nudSpamCount);
			this->Controls->Add(this->lLineDelay);
			this->Controls->Add(this->lSpamCount);
			this->Controls->Add(this->tbPackets);
			this->Controls->Add(this->pProgressCancel);
			this->Controls->Add(this->pSendRecvButtons);
			this->MinimumSize = System::Drawing::Size(457, 179);
			this->Name = L"MainForm";
			this->Text = L"Waty\'s PacketSenderPlz v2.2";
			this->FormClosing += gcnew System::Windows::Forms::FormClosingEventHandler(this, &MainForm::MainForm_FormClosing);
			this->pSendRecvButtons->ResumeLayout(false);
			this->pSendRecvButtons->PerformLayout();
			this->pProgressCancel->ResumeLayout(false);
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->nudSpamCount))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->nudLineDelay))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->nudLoopDelay))->EndInit();
			this->ResumeLayout(false);
			this->PerformLayout();

		}
#pragma endregion
	private:
		Void StartSending(Object^ sender, EventArgs^ e);
		Void bCancel_Click(Object^ sender, EventArgs^ e);
		Void SendAllPackets();
		Void bwSender_DoWork(Object^ sender, DoWorkEventArgs^ e);
		Void bwSender_ProgressChanged(Object^ sender, ProgressChangedEventArgs^ e);
		Void bwSender_RunWorkerCompleted(Object^ sender, RunWorkerCompletedEventArgs^ e);
		Void MainForm_FormClosing(Object^ sender, FormClosingEventArgs^ e);
	};
}
