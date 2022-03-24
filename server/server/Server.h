

#pragma once
#include "Protocol.h"
#include "TypeDef.h"
#include "Global.h"
#include "Singleton.h"
#include "SocketUtil.h"
#include "RoomManager.h"


class ThreadHandler;
class Socket;
class Server : public Singleton<Server>
{
public:
	Server();
	~Server();

public:
	UxVoid	Initialize();
	UxVoid	Run();

public:
	UxVoid CreateClientsSlot();
	SOCKADDR_IN GetServerAddr();
	UxBool IsAvailableId( const std::wstring& name );

public:
	UxVoid SendPacket( UxInt32 id, UxVoid* buff );
	//main
	UxVoid SendPacketLoginOk( UxInt32 id, std::wstring name );
	UxVoid SendPacketLoginDeny( UxInt32 id );
	UxVoid SendPacketJoinGameOk( UxInt32 id );
	UxVoid SendPacketJoinGameDeny( UxInt32 id );
	//lobby
	UxVoid SendPacketMakeRoomOk( UxInt32 id, UxInt32 roomNum );
	UxVoid SendPacketMakeRoomDeny( UxInt32 id );
	UxVoid SendPacketJoinRoomOk( UxInt32 id, std::wstring roomName );
	UxVoid SendPacketRoomUserList( UxInt32 id, UxInt32 who, std::wstring name, UxInt32 character_type, UxBool is_ready );
	UxVoid SendPacketJoinRoomDeny( UxInt32 id );
	UxVoid SendPacketRoomList( UxInt32 id, UxInt32 roomId, std::wstring roomName, UxInt32 participant );
	//game room
	UxVoid SendPacketSelectCharacter( UxInt32 id, UxInt32 who, UxInt32 cha );
	UxVoid SendPacketReady( UxInt32 id, UxInt32 who );
	UxVoid SendPacketUnReady( UxInt32 id, UxInt32 who );
	UxVoid SendPacketLeaveRoom( UxInt32 id, UxInt32 who );
	UxVoid SendPacketGameStart( UxInt32 id, PTC_Player* player_list );
	//in game
	UxVoid SendPacketPosition( UxInt32 id, UxInt32 who, UxSingle x, UxSingle y, UxSingle speed );
	UxVoid SendPacketRotation( UxInt32 id,UxInt32 who, UxSingle rot );
	UxVoid SendPacketAnimation( UxInt32 id, UxInt32 who, UxInt32 anim );
	UxVoid SendPacketHit( UxInt32 id, UxInt32 who, UxInt32 char_type, UxInt32 hit_type );
	UxVoid SendPacketResetCoolTime( UxInt32 id );
	UxVoid SendPacketDeductHeart( UxInt32 id, UxInt32 who, UxInt32 num );
	UxVoid SendPacketDie( UxInt32 id, UxInt32 who );
	UxVoid SendPacketGameOver( UxInt32 id, PTC_Winner* winner );
	UxVoid SendPacketLeftTime( UxInt32 id, UxUInt8 left_time );

public:
	HANDLE	m_iocp;
	std::array<SOCKETINFO*, maxClient> m_clients;

	tbb::concurrent_priority_queue<EVENT> m_timerQueue;
	tbb::concurrent_queue<message> m_roomMsgQueue;

	RoomManager		m_roomManager;

private:
	ThreadHandler*	m_ThreadHandler;

	SocketUtil		m_sockUtil;

	Socket*			m_listenSocket;
	SOCKADDR_IN		m_ServerAddr {};
};

