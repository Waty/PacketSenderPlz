#pragma once
#define GMS
#include <vector>
#include <WinSock.h>
#include <string>

extern void Log(const std::string& msg);
#pragma comment (lib, "Ws2_32.lib")
namespace GMSAddys
{
	const unsigned int MSLockAddy = 0x409440;			// 53 56 8B 74 24 0C 8B D9 8B CE
	const unsigned int MSUnlockAddy = 0x4015A0;			// 8B 01 83 40 04 FF 75 06
	const unsigned int innoHashAddy = 0x14866B0;		// 51 8B 44 24 10 C7 04 24
	const unsigned int FlushSocketAddy = 0x5C4D20;		// 6A FF 68 ? ? ? ? 64 A1 ? ? ? ? 50 83 EC 10 53 55 56 57 A1 ? ? ? ? 33 C4 50 8D 44 24 24 64 A3 ? ? ? ? 8B E9 8B 45 08
	const unsigned int MakeBufferListAddy = 0xA76030;	// 6A FF 68 ? ? ? ? 64 A1 ? ? ? ? 50 83 EC 14 53 55 56 57 A1 ? ? ? ? 33 C4 50 8D 44 24 28 64 A3 ? ? ? ? 8B E9 89 6C 24 1C

	const unsigned int CClientSocketPtr = 0x1A733F8;	// 8B 0D ? ? ? ? 8D 54 24 1C 52 E8 ? ? ? ? 8B 0D

	const unsigned int GameVersion = 149;
}

namespace EMSAddys
{
	const unsigned int MSLockAddy = 0x408C70;			// 53 56 8B 74 24 0C 8B D9 8B CE
	const unsigned int MSUnlockAddy = 0x401350;			// 8B 01 83 40 04 FF 75 06
	const unsigned int innoHashAddy = 0x10BA5C0;		// 51 8B 44 24 10 C7 04 24
	const unsigned int FlushSocketAddy = 0x57DC10;		// 6A FF 68 ? ? ? ? 64 A1 ? ? ? ? 50 83 EC 10 53 55 56 57 A1 ? ? ? ? 33 C4 50 8D 44 24 24 64 A3 ? ? ? ? 8B E9 8B 45 08
	const unsigned int MakeBufferListAddy = 0x8E9EC0;	// 6A FF 68 ? ? ? ? 64 A1 ? ? ? ? 50 83 EC 14 53 55 56 57 A1 ? ? ? ? 33 C4 50 8D 44 24 28 64 A3 ? ? ? ? 8B E9 89 6C 24 1C

	const unsigned int CClientSocketPtr = 0x16CF0A8;	// 8B 0D ? ? ? ? 8D 54 24 1C 52 E8 ? ? ? ? 8B 0D

	const unsigned int GameVersion = 103;
}

#ifdef GMS
using namespace GMSAddys;
#endif // GMS

#ifdef EMS
using namespace EMSAddys;
#endif // EMS



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
	COutPacket(unsigned char* data, unsigned int dwLength) : COutPacket()
	{
		m_lpvSendBuff = data;
		m_uDataLen = dwLength;
	}

	bool m_bLoopback;							// + 0x00
	unsigned char* m_lpvSendBuff;							// + 0x04
	unsigned int m_uDataLen;							// + 0x08
	unsigned int m_uOffset;								// + 0x0C
	bool  m_bIsEncryptedByShanda;				// + 0x10

	void MakeBufferList(ZList<ZRef<void>> *l, unsigned __int16 uSeqBase, unsigned int *puSeqKey, int bEnc, unsigned int dwKey)
	{
		typedef void(__thiscall *MakeBufferList_t)(COutPacket *_this, ZList<ZRef<void>> *l, unsigned __int16 uSeqBase, unsigned int *puSeqKey, int bEnc, unsigned int dwKey);
		MakeBufferList_t MakeBufferList = reinterpret_cast<MakeBufferList_t>(MakeBufferListAddy);
		MakeBufferList(this, l, uSeqBase, puSeqKey, bEnc, dwKey);
	}
};

struct CInPacket
{
	bool m_bLoopback;							// + 0x00
	int m_nState;								// + 0x04
	unsigned char* m_lpbRecvBuff;							// + 0x08
	unsigned int m_uLength;								// + 0x0C
	unsigned int m_uRawSeq;								// + 0x10
	unsigned int m_uDataLen;							// + 0x14
	unsigned int m_uOffset;								// + 0x18
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
	__inline ZSynchronizedHelper(T* lock)
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

static auto CIGCipher__innoHash = reinterpret_cast<unsigned int(__cdecl *)(char *pSrc, int nLen, unsigned int *pdwKey)>(innoHashAddy);
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
		reinterpret_cast<void(__thiscall*)(CClientSocket*)>(FlushSocketAddy)(this);
	}

	void SendPacket(COutPacket& oPacket)
	{
		ZSynchronizedHelper<ZFatalSection> lock(&m_lockSend);

		if (m_sock._m_hSocket != 0 && m_sock._m_hSocket != 0xFFFFFFFF && m_ctxConnect.lAddr._m_uCount == 0)
		{
			oPacket.MakeBufferList(&m_lpSendBuff, GameVersion, &m_uSeqSnd, 1, m_uSeqSnd);
			m_uSeqSnd = CIGCipher__innoHash(reinterpret_cast<char*>(&m_uSeqSnd), 4, 0);
			Flush();
		}
	}
};
#ifdef GMS
static_assert(sizeof(CClientSocket) == 0x98, "CClientSocket is the wrong size!");
#endif // GMS

#ifdef EMS
static_assert(sizeof(CClientSocket) == 0x9C, "CClientSocket is the wrong size!");
#endif // EMS
