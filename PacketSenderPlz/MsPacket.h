#pragma once

class MsPacket
{
	std::string m_error;
	std::string m_source;
	std::vector<unsigned char> m_data;
	bool m_bShouldBeParsed;
	static unsigned int dwMainThreadID;

	template <typename T>
	void Encode(T data);

public:
	MsPacket();
	bool IsConnected();

	void Encode1(uint8_t data);
	void Encode2(uint16_t data);
	void Encode4(uint32_t data);
	void Encode8(uint64_t data);
	void EncodeString(std::string data);

	bool Parse(std::string& source);
	bool Recv();
	bool Send();

	std::string ToString();
	std::string GetError();
};
