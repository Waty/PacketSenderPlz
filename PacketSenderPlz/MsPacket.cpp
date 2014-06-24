#include "MsPacket.h"

#include <Windows.h>
#include <algorithm>
#include <iomanip>
#include <sstream>
#include <TlHelp32.h>


extern void Log(const std::string& message);

MsPacket::MsPacket() : m_bShouldBeParsed(true)
{
	if (dwMainThreadID == NULL) GetMapleThreadId();
}

void MsPacket::Encode1(uint8_t data)
{
	m_data.push_back(data);
}

void MsPacket::Encode2(uint16_t data)
{
	m_data.push_back(data & 0xFF);
	m_data.push_back((data >> 8) & 0xFF);
}

void MsPacket::Encode4(uint32_t data)
{
	m_data.push_back(data & 0xFF);
	m_data.push_back((data >> 8) & 0xFF);
	m_data.push_back((data >> 16) & 0xFF);
	m_data.push_back((data >> 24) & 0xFF);
}

void MsPacket::Encode8(uint64_t data)
{
	m_data.push_back(data & 0xFF);
	m_data.push_back((data >> 8) & 0xFF);
	m_data.push_back((data >> 16) & 0xFF);
	m_data.push_back((data >> 24) & 0xFF);
	m_data.push_back((data >> 32) & 0xFF);
	m_data.push_back((data >> 40) & 0xFF);
	m_data.push_back((data >> 48) & 0xFF);
	m_data.push_back((data >> 56) & 0xFF);
}

void MsPacket::EncodeString(std::string data)
{
	Encode2(data.size());
	for (size_t i = 0; i < data.size(); i++) m_data.push_back(data[i]);
}

void MsPacket::GetMapleThreadId()
{
#define MAKEULONGLONG(ldw, hdw) ((ULONGLONG(hdw) << 32) | ((ldw) & 0xFFFFFFFF))

	DWORD dwProcID = GetCurrentProcessId();
	ULONGLONG ullMinCreateTime = MAXULONGLONG;

	HANDLE hThreadSnap = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
	if (hThreadSnap != INVALID_HANDLE_VALUE)
	{
		THREADENTRY32 th32;
		th32.dwSize = sizeof(THREADENTRY32);
		BOOL bOK = TRUE;
		for (bOK = Thread32First(hThreadSnap, &th32); bOK; bOK = Thread32Next(hThreadSnap, &th32))
		{
			if (th32.th32OwnerProcessID == dwProcID)
			{
				HANDLE hThread = OpenThread(THREAD_QUERY_INFORMATION, TRUE, th32.th32ThreadID);
				if (hThread)
				{
					FILETIME afTimes[4] = { 0 };
					if (GetThreadTimes(hThread, &afTimes[0], &afTimes[1], &afTimes[2], &afTimes[3]))
					{
						ULONGLONG ullTest = MAKEULONGLONG(afTimes[0].dwLowDateTime, afTimes[0].dwHighDateTime);
						if (ullTest && ullTest < ullMinCreateTime)
						{
							ullMinCreateTime = ullTest;
							dwMainThreadID = th32.th32ThreadID; // let it be main... :)
						}
					}
					CloseHandle(hThread);
				}
			}
		}
#ifndef UNDER_CE
		CloseHandle(hThreadSnap);
#else
		CloseToolhelp32Snapshot(hThreadSnap);
#endif
	}
}

bool MsPacket::IsConnected()
{
	try { return *reinterpret_cast<void**>(CClientSocketPtr) != nullptr; }
	catch (...) { return false; }
}

bool MsPacket::Parse(std::string& data)
{
	m_source = data;
	//Removing all spaces from the packet
	m_source.erase(std::remove(m_source.begin(), m_source.end(), ' '), m_source.end());
	if (m_source.size() % 2 != 0)
	{
		m_error = "The total length of the packet was not an even number.\n"
			"Because I'm lazy, the parser isn't able to understand that you mean '0F' when you type 'F', "
			"so please prefix all small bytes with a 0.\n";
		return false;
	}

	if (m_source.size() == 0)
	{
		m_error = "The input string didn't contain a single valid hexadecimal byte...\n";
		return false;
	}

	m_data.clear();
	m_data.reserve(m_source.size() / 2);
	m_bShouldBeParsed = false;
	for (int i = 0, size = m_source.size(); i < size; i += 2)
	{
		char c1 = m_source[i];
		char c2 = m_source[i + 1];

		auto isValidByte = [this](char& c)
		{
			if (!isxdigit(c))
			{
				std::stringstream ss;
				if (c == '?' || c == '*')
				{
					//The packet contains random bytes so we need to do the parsing every time we send the packet
					m_bShouldBeParsed = true;
					ss << std::uppercase << std::hex << rand() % 0xF;
					ss >> c;
				}
				else
				{
					ss << std::uppercase << std::hex << c;
					m_error = "It contained an invalid character: " + ss.str();
					return false;
				}
			}
			return true;
		};
		if (isValidByte(c1) && isValidByte(c2))
		{
			int b;
			std::stringstream ss;
			ss << std::hex << c1 << c2;
			ss >> b;
			m_data.push_back(b);
		}
		else return false;
	}
	return true;
}

bool MsPacket::Recv()
{
	m_error = "Not implemented :-(";
	return false;
}

bool MsPacket::Send()
{
#ifdef _DEBUG
	if (m_bShouldBeParsed)
	{
		if (!Parse(m_source)) return false;
	}
	Log("Sent " + ToString());
	return true;

#else

	//The packet needs to have atleast the header if we want to send it
	if (!IsConnected())
	{
		m_error = "CClientSocket isn't connected\n"
			"You are probably hitting the Send button at the play screen, or MS just crashed...";
		return false;
	}
	if (m_bShouldBeParsed && !Parse(m_source)) return false;

	//Spoofs threadId
	//__writefsdword(0x6B8, dwMainThreadID);

	COutPacket p;
	p.m_lpvSendBuff = &m_data[0];
	p.m_uDataLen = m_data.size();

	try {
		Log("Going to send " + ToString());
		(*reinterpret_cast<CClientSocket**>(CClientSocketPtr))->SendPacket(p);
		Log("Finished Sending :-)");
		
		//SendPacket(p);
		return true; 
	
	}
	catch (...) { return false; }
#endif
}

std::string MsPacket::ToString()
{
	std::stringstream ss;
	ss << std::uppercase << std::hex;
	for (BYTE b : m_data) ss << std::setw(2) << std::setfill('0') << int(b) << " ";
	std::string result = ss.str();
	result.pop_back();
	return result;
}

std::string MsPacket::GetError()
{
	return m_error;
}

uint32_t MsPacket::dwMainThreadID = 0;
