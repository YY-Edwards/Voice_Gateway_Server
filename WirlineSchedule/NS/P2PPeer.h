#pragma once
#include "NSStruct.h"
class CP2PNet;
class NSLog;
class CP2PPeer
{
public:
	CP2PPeer(p2p_peer_build_param_t* p);
	~CP2PPeer();
private:
	unsigned long m_peerId;
	CP2PNet* m_p2pNet;
	StartNetParam m_netParam;
	SOCKADDR_IN  m_socket;
	
	DWORD Build_LE_PEER_REGISTRATION_REQUEST(CHAR* pPacket, T_LE_PROTOCOL_94* pData);
	DWORD Build_LE_PEER_REGISTRATION_RESPONSE(CHAR* pPacket, T_LE_PROTOCOL_95* pData);
	DWORD Build_LE_PEER_KEEP_ALIVE_RESPONSE(CHAR* pPacket, T_LE_PROTOCOL_99* pData);
	DWORD Build_LE_PEER_KEEP_ALIVE_REQUEST(CHAR* pPacket, T_LE_PROTOCOL_98* pData);
public :
	void SEND_LE_PEER_REGISTRATION_REQUEST(work_item_t* w);
	void SEND_LE_PEER_REGISTRATION_RESPONSE(work_item_t* w);
	void SEND_LE_PEER_KEEP_ALIVE_RESPONSE(work_item_t* w);
	void SEND_LE_PEER_KEEP_ALIVE_REQUEST(work_item_t* w, unsigned long timing = 0);
	unsigned long PeerId();
	void LE_PEER_REGISTRATION();
	void Handle_NetPack(char Opcode);
};

