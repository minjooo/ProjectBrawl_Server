

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
	UxBool IsAvailableId( const std::string& name );

public:
	UxVoid SendPacket( UxInt32 id, UxVoid* buff );
	//main
	UxVoid SendPacketLoginOk( UxInt32 id );
	UxVoid SendPacketLoginDeny( UxInt32 id );
	UxVoid SendPacketJoinGameOk( UxInt32 id );
	UxVoid SendPacketJoinGameDeny( UxInt32 id );
	//lobby
	UxVoid SendPacketMakeRoomOk( UxInt32 id, UxInt32 roomNum );
	UxVoid SendPacketMakeRoomDeny( UxInt32 id );
	UxVoid SendPacketJoinRoomOk( UxInt32 id, std::string roomName );
	UxVoid SendPacketRoomUserList( UxInt32 id, UxInt32 who, std::string name, UxInt8 character_type, UxInt8 is_ready );
	UxVoid SendPacketJoinRoomDeny( UxInt32 id );
	UxVoid SendPacketRoomList( UxInt32 id, UxInt32 totalNum, PTC_Room* room_list );
	//game room
	UxVoid SendPacketSelectCharacter( UxInt32 id, UxInt32 who, UxInt8 cha );
	UxVoid SendPacketReady( UxInt32 id, UxInt32 who );
	UxVoid SendPacketUnReady( UxInt32 id, UxInt32 who );
	UxVoid SendPacketLeaveRoom( UxInt32 id, UxInt32 who );
	UxVoid SendPacketGameStart( UxInt32 id );
	//in game
	UxVoid SendPacketPosition( UxInt32 id, UxInt32 who, UxSingle x, UxSingle y, UxSingle speed );
	UxVoid SendPacketRotation( UxInt32 id,UxInt32 who, UxSingle rot );
	UxVoid SendPacketAnimation( UxInt32 id, UxInt32 who, UxInt8 anim );
	UxVoid SendPacketHit( UxInt32 id );//일단 보류
	UxVoid SendPacketDeductHeart( UxInt32 id, UxInt32 who, UxInt32 num );
	UxVoid SendPacketDie( UxInt32 id, UxInt32 who );
	UxVoid SendPacketGameOver( UxInt32 id, PTC_Winner* winner );
	UxVoid SendPacketLeftTime( UxInt32 id, UxUInt8 left_time );

public:
	HANDLE	m_iocp;
	std::array<SOCKETINFO*, maxClient> m_clients;

	tbb::concurrent_priority_queue<EVENT> m_timerQueue;
	tbb::concurrent_queue<message> m_roomMsgQueue;

	RoomManager				m_roomManager;

private:
	ThreadHandler* m_ThreadHandler;

	SocketUtil				m_sockUtil;

	std::shared_ptr<Socket> m_listenSocket;
	SOCKADDR_IN				m_ServerAddr {};
};

