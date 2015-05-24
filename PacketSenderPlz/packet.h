#pragma once

class packet
{
	std::string error_;
	std::string source_;
	std::vector<unsigned char> data_;
	bool should_be_parsed_;

	static uint32_t main_thread_id_;

	template <typename T>
	void encode(T data);

public:
	packet();
	packet(uint16_t header);
	bool is_connected();

	void encode1(uint8_t data);
	void encode2(uint16_t data);
	void encode4(uint32_t data);
	void encode8(uint64_t data);
	void encode_string(std::string data);
	void encode_data(std::initializer_list<uint8_t> data);

	bool parse(std::string& source);
	bool receive();
	bool send();

	std::string to_string();
	std::string get_error();
};
