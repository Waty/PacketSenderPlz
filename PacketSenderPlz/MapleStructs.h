#pragma once
#include <vector>
#include <stdint.h>
#include <WinSock.h>
#include <string>

extern void Log(const std::string& msg);
#pragma comment (lib, "Ws2_32.lib")

#define GMS
#ifdef GMS
const uint32_t MSLockAddy = 0x409440;			// 53 56 8B 74 24 0C 8B D9 8B CE
const uint32_t MSUnlockAddy = 0x4015A0;			// 8B 01 83 40 04 FF 75 06
const uint32_t innoHashAddy = 0x14866B0;		// 51 8B 44 24 10 C7 04 24
const uint32_t FlushSocketAddy = 0x5C4D20;		// 6A FF 68 ? ? ? ? 64 A1 ? ? ? ? 50 83 EC 10 53 55 56 57 A1 ? ? ? ? 33 C4 50 8D 44 24 24 64 A3 ? ? ? ? 8B E9 8B 45 08
const uint32_t MakeBufferListAddy = 0xA76030;	// 6A FF 68 ? ? ? ? 64 A1 ? ? ? ? 50 83 EC 14 53 55 56 57 A1 ? ? ? ? 33 C4 50 8D 44 24 28 64 A3 ? ? ? ? 8B E9 89 6C 24 1C

const uint32_t CClientSocketPtr = 0x1A733F8;	// 8B 0D ? ? ? ? 8D 54 24 1C 52 E8 ? ? ? ? 8B 0D

#elif EMS

typedef void(__thiscall *ProcessPacket_t)(void* lpvEcx, CInPacket& iPacket);
ProcessPacket_t ProcessPacket = reinterpret_cast<ProcessPacket_t>(ProcessPacketAddy);

const uint32_t SendPacketAddy = 0x478780;
typedef void(__cdecl *SendPacket_t)(COutPacket& oPacket);
SendPacket_t SendPacket = reinterpret_cast<SendPacket_t>(SendPacketAddy);
#endif

struct ZSocketBase
{
	unsigned int _m_hSocket;
};

template <class T> struct ZList
{
	virtual ~ZList<T>();		//0x00
	void* baseclass_4;			//0x04
	unsigned int _m_uCount;		//0x08
	T* _m_pHead;				//0x0C
	T* _m_pTail;				//0x10	
};								//0x14 
static_assert(sizeof(ZList<void>) == 0x14, "ZList is the wrong size");

template <class T> struct ZRef
{
	void* vfptr;
	T* data;
};

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

	void MakeBufferList(ZList<ZRef<void>> *l, unsigned __int16 uSeqBase, unsigned int *puSeqKey, int bEnc, unsigned int dwKey)
	{
		typedef void(__thiscall *MakeBufferList_t)(COutPacket *_this, ZList<ZRef<void>> *l, unsigned __int16 uSeqBase, unsigned int *puSeqKey, int bEnc, unsigned int dwKey);
		MakeBufferList_t MakeBufferList = reinterpret_cast<MakeBufferList_t>(MakeBufferListAddy);
		MakeBufferList(this, l, uSeqBase, puSeqKey, bEnc, dwKey);
	}
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

struct ZFatalSectionData
{
	void *_m_pTIB;									// + 0x00
	int _m_nRef;									// + 0x04
};

struct ZFatalSection : public ZFatalSectionData
{

};

template<class T> struct ZSynchronizedHelper
{
public:
	ZSynchronizedHelper(T* lock)
	{
		typedef void(__thiscall * Constructor_t)(ZSynchronizedHelper<T>* _this, T* lock);
		Constructor_t Constructor = reinterpret_cast<Constructor_t>(MSLockAddy);
		Constructor(this, lock);
	}

	~ZSynchronizedHelper()
	{
		typedef void(__thiscall *Destructor_t)(ZSynchronizedHelper<T>* _this);
		Destructor_t Destructor = reinterpret_cast<Destructor_t>(MSUnlockAddy);
		Destructor(this);
	}

private:
	T* m_pLock;
};

typedef unsigned int(__cdecl *CIGCipher__innoHash_t)(char *pSrc, int nLen, unsigned int *pdwKey);
static CIGCipher__innoHash_t CIGCipher__innoHash = reinterpret_cast<CIGCipher__innoHash_t>(innoHashAddy);
struct CClientSocket
{
	struct CONNECTCONTEXT
	{
		ZList<sockaddr_in> lAddr;
		void *posList;
		int bLogin;
	};

	virtual ~CClientSocket();
	void* ___u1;
	ZSocketBase m_sock;
	CONNECTCONTEXT m_ctxConnect;
	sockaddr_in m_addr;
	int m_tTimeout;
#ifdef EMS
	void* unknown;					//ZList<ZInetAddr>::'vftable'
#endif
	ZList<ZRef<void> > m_lpRecvBuff; //ZList<ZRef<ZSocketBuffer> >
	ZList<ZRef<void> > m_lpSendBuff; //ZList<ZRef<ZSocketBuffer> >
	CInPacket m_packetRecv;
	ZFatalSection m_lockSend;
	unsigned int m_uSeqSnd;
	unsigned int m_uSeqRcv;
	char* m_URLGuestIDRegistration;
	int m_bIsGuestID;

	void Flush()
	{
		auto Flush = reinterpret_cast<void(__thiscall*)(CClientSocket*)>(FlushSocketAddy);
		Flush(this);
	}

	void SendPacket(COutPacket& oPacket)
	{
		ZSynchronizedHelper<ZFatalSection> lock(&m_lockSend);

		if (m_sock._m_hSocket != 0 && m_sock._m_hSocket != 0xFFFFFFFF && m_ctxConnect.lAddr._m_uCount == 0)
		{
			oPacket.MakeBufferList(&m_lpSendBuff, 149, &m_uSeqSnd, 1, m_uSeqSnd);
			m_uSeqSnd = CIGCipher__innoHash(reinterpret_cast<char*>(&m_uSeqSnd), 4, 0);
			Flush();
		}
	}
};

static_assert(sizeof(CClientSocket) == 0x98, "CCLientSocket is the wrong size!");
//typedef void(__thiscall *CClientSocket__SendPacket_t)(CClientSocket* _this, COutPacket* oPacket);
