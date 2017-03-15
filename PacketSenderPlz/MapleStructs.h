#pragma once

const uint32_t GameVersion = 183;
const uint32_t MSLockAddy = 0x5E5210; // 55 8B EC 56 8B 75 08 57 8B F9
const uint32_t MSUnlockAddy = 0x60B790; // 8B 01 83 40 04 FF 75 06
const uint32_t innoHashAddy = 0xBEAAE0; // 55 8B EC 51 8B 4D 10 8D 45
const uint32_t FlushSocketAddy = 0xDC7150; // 55 8B EC 6A FF 68 48 8C 2F 02 64 A1 00 00 00 00 50 83 EC 18 A1 E8 58 AC 02 33 C5 50
const uint32_t MakeBufferListAddy = 0x9BB2E0; // 55 8B EC 6A FF 68 A8 B3 29 02 64 A1 00 00 00 00 50 83 EC 1C 53 56 57 A1 E8 58 AC 02 33 C5 50 8D 45 F4 64 A3 00 00 00 00 89 4D E8 8B 41 08 3D 00 FF 00 00 8D 50
const uint32_t CClientSocketPtr = 0x2AF17B4; // 8B 0D ? ? ? ? 85 ? 74 ? 8D ? ? ? ? ? ? 19 00


struct ZSocketBase
{
	unsigned int _m_hSocket;
};

template <class T>
struct ZList
{
	virtual ~ZList<T>() = delete;
	void* baseclass_4;
	unsigned int _m_uCount;
	T* _m_pHead;
	T* _m_pTail;
};

static_assert(sizeof(ZList<uint32_t>) == 0x14, "ZList is the wrong size");

template <class T>
struct ZRef
{
	void* vfptr;
	T* data;
};

#pragma pack( push, 1 )
struct COutPacket
{
	COutPacket() : m_bLoopback(false), m_lpvSendBuff(nullptr), m_uDataLen(0), m_uOffset(0), m_bIsEncryptedByShanda(false)
	{
	}

	COutPacket(unsigned char* data, unsigned int dwLength) : COutPacket()
	{
		m_lpvSendBuff = data;
		m_uDataLen = dwLength;
	}

	bool m_bLoopback; // + 0x00
	unsigned char* m_lpvSendBuff; // + 0x04
	unsigned int m_uDataLen; // + 0x08
	unsigned int m_uOffset; // + 0x0C
	bool m_bIsEncryptedByShanda; // + 0x10

	void MakeBufferList(ZList<ZRef<void>>* l, unsigned __int16 uSeqBase, unsigned int* puSeqKey, int bEnc, unsigned int dwKey)
	{
		typedef void(__thiscall *MakeBufferList_t)(COutPacket* _this, ZList<ZRef<void>>* l, unsigned __int16 uSeqBase, unsigned int* puSeqKey, int bEnc, unsigned int dwKey);
		MakeBufferList_t MakeBufferList = reinterpret_cast<MakeBufferList_t>(MakeBufferListAddy);
		MakeBufferList(this, l, uSeqBase, puSeqKey, bEnc, dwKey);
	}
};

struct CInPacket
{
	bool m_bLoopback; // + 0x00
	int m_nState; // + 0x04
	unsigned char* m_lpbRecvBuff; // + 0x08
	unsigned int m_uLength; // + 0x0C
	unsigned int m_uRawSeq; // + 0x10
	unsigned int m_uDataLen; // + 0x14
	unsigned int m_uOffset; // + 0x18
};

#pragma pack( pop )

struct ZFatalSectionData
{
	void* _m_pTIB; // + 0x00
	int _m_nRef; // + 0x04
};

struct ZFatalSection : ZFatalSectionData
{
};

template <class T>
struct ZSynchronizedHelper
{
	explicit __inline ZSynchronizedHelper(T* lock)
	{
		reinterpret_cast<void(__thiscall*)(ZSynchronizedHelper<T>*, T*)>(MSLockAddy)(this, lock);
	}

	__inline ~ZSynchronizedHelper()
	{
		reinterpret_cast<void(__thiscall*)(ZSynchronizedHelper<T>*)>(MSUnlockAddy)(this);
	}

private:
	T* m_pLock;
};

static auto CIGCipher__innoHash = reinterpret_cast<unsigned int(__cdecl *)(char* pSrc, int nLen, unsigned int* pdwKey)>(innoHashAddy);
struct CClientSocket
{
	struct CONNECTCONTEXT
	{
		ZList<sockaddr_in> lAddr;
		void* posList;
		int bLogin;

		~CONNECTCONTEXT() = delete;
	};

	virtual ~CClientSocket() = delete;
	void* _unknown1;
	HWND* m_hWnd;
	ZSocketBase m_sock;
	CONNECTCONTEXT m_ctxConnect;
	sockaddr_in m_addr;
	int m_tTimeout;
	void* _unknown2;
	ZList<ZRef<void>> m_lpRecvBuff; //ZList<ZRef<ZSocketBuffer> >
	ZList<ZRef<void>> m_lpSendBuff; //ZList<ZRef<ZSocketBuffer> >
	CInPacket m_packetRecv;
	ZFatalSection m_lockSend;
	unsigned int m_uSeqSnd;
	unsigned int m_uSeqRcv;

	void Flush()
	{
		reinterpret_cast<void(__thiscall*)(CClientSocket*)>(FlushSocketAddy)(this);
	}

	void SendPacket(COutPacket& oPacket)
	{
		ZSynchronizedHelper<ZFatalSection> lock(&m_lockSend);

		if (m_sock._m_hSocket != 0 && m_sock._m_hSocket != 0xFFFFFFFF && m_ctxConnect.lAddr._m_uCount == 0)
		{
			oPacket.MakeBufferList(&m_lpSendBuff, GameVersion, &m_uSeqSnd, 1, m_uSeqSnd);
			m_uSeqSnd = CIGCipher__innoHash(reinterpret_cast<char*>(&m_uSeqSnd), 4, nullptr);
			Flush();
		}
	}
};

static_assert(sizeof(CClientSocket) == 0x98, "CClientSocket is the wrong size!");
