#include "stdafx.h"
#include "packet.h"
#include "MapleStructs.h"

extern void log(const std::string& message);

packet::packet() : should_be_parsed_(true) {}

void packet::encode1(uint8_t data)
{
	return encode<uint8_t>(data);
}

void packet::encode2(uint16_t data)
{
	return encode<uint16_t>(data);
}

void packet::encode4(uint32_t data)
{
	return encode<uint32_t>(data);
}

void packet::encode8(uint64_t data)
{
	return encode<uint64_t>(data);
}

void packet::encode_string(std::string data)
{
	encode2(data.size());
	data_.insert(data_.end(), data.begin(), data.end());
}

template <typename T>
void packet::encode(T data)
{
	auto ptr = reinterpret_cast<unsigned char*>(&data);
	data_.insert(data_.end(), ptr, ptr + sizeof(T));
}

bool packet::is_connected()
{
	try
	{
		return *reinterpret_cast<void**>(CClientSocketPtr) != nullptr;
	}
	catch (...)
	{
		return false;
	}
}

bool packet::parse(std::string& data)
{
	source_ = data;
	//Removing all spaces from the packet
	source_.erase(remove(source_.begin(), source_.end(), ' '), source_.end());
	if (source_.size() % 2 != 0)
	{
		error_ = "The total length of the packet was not an even number.\n"
		"Because I'm lazy, the parser isn't able to understand that you mean '0F' when you type 'F', "
		"so please prefix all small bytes with a 0.\n";
		return false;
	}

	if (source_.size() == 0)
	{
		error_ = "The input string didn't contain a single valid hexadecimal byte...\n";
		return false;
	}

	data_.clear();
	data_.reserve(source_.size() / 2);
	should_be_parsed_ = false;
	for (int i = 0, size = source_.size(); i < size; i += 2)
	{
		char c1 = source_[i];
		char c2 = source_[i + 1];

		auto is_valid_byte = [this](char& c)
		{
			if (!isxdigit(c))
			{
				std::stringstream ss;
				if (c == '?' || c == '*')
				{
					//The packet contains random bytes so we need to do the parsing every time we send the packet
					should_be_parsed_ = true;
					ss << std::uppercase << std::hex << rand() % 0xF;
					ss >> c;
				}
				else
				{
					ss << std::uppercase << std::hex << c;
					error_ = "It contained an invalid character: " + ss.str();
					return false;
				}
			}
			return true;
		};

		if (is_valid_byte(c1) && is_valid_byte(c2))
		{
			int b;
			std::stringstream ss;
			ss << std::hex << c1 << c2;
			ss >> b;
			data_.push_back(b);
		}
		else return false;
	}
	return true;
}

bool packet::receive()
{
	error_ = "Not implemented :-(";
	return false;
}

bool packet::send()
{
#ifdef _DEBUG
	if (should_be_parsed_ && !parse(source_)) return false;

	log("Sent " + to_string());
	return true;
#else

	//The packet needs to have atleast the header if we want to send it
	if (!is_connected())
	{
		error_ = "CClientSocket isn't connected\n"
			"You are probably hitting the Send button at the play screen, or MS just crashed...";
		return false;
	}
	if (should_be_parsed_ && !parse(source_)) return false;

	COutPacket p;
	p.m_lpvSendBuff = &data_[0];
	p.m_uDataLen = data_.size();

	try { (*reinterpret_cast<CClientSocket**>(CClientSocketPtr))->SendPacket(p); return true; }
	catch (...) { return false; }
#endif
}

std::string packet::to_string()
{
	std::stringstream ss;
	ss << std::uppercase << std::hex;
	for (uint8_t b : data_) ss << std::setw(2) << std::setfill('0') << static_cast<int>(b) << ' ';

	std::string result = ss.str();
	result.pop_back();
	return result;
}

std::string packet::get_error()
{
	return error_;
}

uint32_t packet::main_thread_id_ = 0;
