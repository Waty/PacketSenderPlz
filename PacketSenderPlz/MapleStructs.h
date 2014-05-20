#pragma once
#include <vector>
#include <stdint.h>

#pragma pack( push, 1 )

struct COutPacket
{
	COutPacket() : m_bLoopback(false), m_bIsEncryptedByShanda(false), m_uOffset(0) { }
	COutPacket(uint8_t* data, uint32_t dwLength) : COutPacket()
	{
		m_lpvSendBuff = data;
		m_uDataLen = dwLength;
	}

	int32_t  m_bLoopback;							// + 0x00
	uint8_t* m_lpvSendBuff;							// + 0x04
	uint32_t m_uDataLen;							// + 0x08
	uint32_t m_uOffset;								// + 0x0C
	int32_t  m_bIsEncryptedByShanda;				// + 0x10
};

struct CInPacket
{
	int32_t m_bLoopback;							// + 0x00
	int32_t m_nState;								// + 0x04
	uint8_t* m_lpbRecvBuff;							// + 0x08
	uint32_t m_uLength;								// + 0x0C
	uint32_t m_uRawSeq;								// + 0x10
	uint32_t m_uDataLen;							// + 0x14
	uint32_t m_uOffset;								// + 0x18
};

#pragma pack( pop )
