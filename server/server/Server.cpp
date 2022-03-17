
#include "Header.h"
#include "ThreadHandler.h"
#include "Socket.h"
#include "SocketAddress.h"
#include "Server.h"


Server::Server()
{
	Initialize();
}

Server::~Server()
{

}

UxVoid Server::Initialize()
{
	//client 생성
	CreateClientsSlot();

	std::wcout.imbue( std::locale( "korean" ) );

	if ( !m_sockUtil.StaticInitialize() ) exit( 1 );

	m_iocp = CreateIoCompletionPort( INVALID_HANDLE_VALUE, NULL, NULL, 0 );

	m_listenSocket = m_sockUtil.CreateSocket( ESocketAddressFamily::INET );

	SocketAddress serverAddr { INADDR_ANY, SERVER_PORT };

	m_listenSocket->Bind( serverAddr );
	m_listenSocket->Listen();
}

void Server::Run()
{
	m_ThreadHandler = new ThreadHandler;
	m_ThreadHandler->CreateThreads();

	SocketAddress clientAddress {};
	std::shared_ptr<Socket> clientSocket {};
	DWORD flags {};

	while ( true ) 
	{
		clientSocket = m_listenSocket->Accept( clientAddress );

		int user_id {};
		for ( int i = 2; i < maxClient; ++i )
		{
			if ( !m_clients[i]->isConnected ) 
			{
				user_id = i;
				break;
			}
		}
		std::cout << "[" << user_id << "] accept!\n";
		SOCKETINFO* new_player = new SOCKETINFO;
		new_player->socket = clientSocket;

		new_player->recv_over.wsabuf[0].len = maxBuffer;
		new_player->recv_over.wsabuf[0].buf = new_player->recv_over.net_buf;

		new_player->recv_over.ev_type = EEventType::RECV;

		new_player->curr_packet_size = 0;
		new_player->prev_packet_data = 0;

		new_player->isConnected = true;

		m_clients[user_id] = new_player;

		CreateIoCompletionPort( reinterpret_cast< HANDLE >( clientSocket->GetSocket() ), m_iocp, user_id, 0 );

		memset( &m_clients[user_id]->recv_over.over, 0, sizeof( WSAOVERLAPPED ) );
		clientSocket->WSAReceive( m_clients[user_id]->recv_over.wsabuf, 1, NULL, &flags, &m_clients[user_id]->recv_over.over );
		clientSocket = nullptr;
	}
}

UxVoid Server::SendPacket( UxInt32 id, UxVoid* buff )
{
	UxInt8* packet = reinterpret_cast< UxInt8* >( buff );
	UxInt8 packet_size = ( UxInt8 )packet[0];
	OVER_EX* send_over = new OVER_EX;
	memset( send_over, 0, sizeof( OVER_EX ) );
	send_over->ev_type = EEventType::SEND;
	memcpy( send_over->net_buf, packet, packet_size );
	send_over->wsabuf[0].len = packet_size;
	send_over->wsabuf[0].buf = send_over->net_buf;

	m_clients[id]->socket->WSASend( send_over->wsabuf, 1, 0, 0, &send_over->over );
}

//main
UxVoid Server::SendPacketLoginOk( UxInt32 id )
{
#ifdef LOG_ON
	std::cout << "[" << id << "] send login ok!\n";
#endif
	scPacketLoginOk packet;
	packet.size = sizeof( packet );
	packet.type = SC_LOGIN_OK;
	SendPacket( id, &packet );
}

UxVoid Server::SendPacketLoginDeny( UxInt32 id )
{
#ifdef LOG_ON
	std::cout << "[" << id << "] send login deny!\n";
#endif
	scPacketLoginDeny packet;
	packet.size = sizeof( packet );
	packet.type = SC_LOGIN_DENY;
	SendPacket( id, &packet );
}

UxVoid Server::SendPacketJoinGameOk( UxInt32 id )
{
#ifdef LOG_ON
	std::cout << "[" << id << "] send join game ok!\n";
#endif
	scPacketJoinGameOk packet;
	packet.size = sizeof( packet );
	packet.type = SC_JOIN_GAME_OK;
	SendPacket( id, &packet );
}

UxVoid Server::SendPacketJoinGameDeny( UxInt32 id )
{
#ifdef LOG_ON
	std::cout << "[" << id << "] send join game deny!\n";
#endif
	scPacketJoinGameDeny packet;
	packet.size = sizeof( packet );
	packet.type = SC_JOIN_GAME_DENY;
	SendPacket( id, &packet );
}

//lobby
UxVoid Server::SendPacketMakeRoomOk( UxInt32 id, UxInt32 roomNum )
{
#ifdef LOG_ON
	std::cout << "[" << id << "] send make room ok!\n";
#endif
	scPacketMakeRoomOk packet;
	packet.size = sizeof( packet );
	packet.type = SC_MAKE_ROOM_OK;
	packet.roomNum = roomNum;
	SendPacket( id, &packet );
}

UxVoid Server::SendPacketMakeRoomDeny( UxInt32 id )
{
#ifdef LOG_ON
	std::cout << "[" << id << "] send make room deny!\n";
#endif
	scPacketMakeRoomDeny packet;
	packet.size = sizeof( packet );
	packet.type = SC_MAKE_ROOM_DENY;
	SendPacket( id, &packet );
}

UxVoid Server::SendPacketJoinRoomOk( UxInt32 id, std::string roomName )
{
#ifdef LOG_ON
	std::cout << "[" << id << "] send join room ok!\n";
#endif
	scPacketJoinRoomOk packet;
	packet.size = sizeof( packet );
	packet.type = SC_JOIN_ROOM_OK;
	strcpy_s( packet.roomName, roomName.c_str() );
	SendPacket( id, &packet );
}

UxVoid Server::SendPacketRoomUserList( UxInt32 id, UxInt32 who, std::string name, UxInt8 character_type )
{
#ifdef LOG_ON
	std::cout << "[" << id << "] send room user list ok!\n";
#endif
	scPacketRoomUserList packet;
	packet.size = sizeof( packet );
	packet.type = SC_ROOM_USER_LIST;
	packet.id = who;
	packet.character_type = character_type;
	strcpy_s( packet.name, name.c_str() );
	SendPacket( id, &packet );
}

UxVoid Server::SendPacketJoinRoomDeny( UxInt32 id )
{
#ifdef LOG_ON
	std::cout << "[" << id << "] send join room deny!\n";
#endif
	scPacketJoinRoomDeny packet;
	packet.size = sizeof( packet );
	packet.type = SC_JOIN_ROOM_DENY;
	SendPacket( id, &packet );
}

UxVoid Server::SendPacketRoomList( UxInt32 id, UxInt32 totalNum, PTC_Room* room_list )
{
#ifdef LOG_ON
	std::cout << "[" << id << "] send room list!\n";
#endif
	scPacketRoomList packet;
	packet.size = sizeof( packet );
	packet.type = SC_ROOM_LIST;
	packet.totalNum = totalNum;
	memcpy( &packet.room_list, room_list, sizeof( packet.room_list ) );
	SendPacket( id, &packet );
}

//game room
UxVoid Server::SendPacketSelectCharacter( UxInt32 id, UxInt32 who, UxInt8 cha )
{
#ifdef LOG_ON
	std::cout << "[" << id << "] send select character!\n";
#endif
	scPacketSelectCharacter packet;
	packet.size = sizeof( packet );
	packet.type = SC_SELECT_CHARACTER;
	packet.id = who;
	packet.character = cha;
	SendPacket( id, &packet );
}

UxVoid Server::SendPacketReady( UxInt32 id, UxInt32 who )
{
#ifdef LOG_ON
	std::cout << "[" << id << "] send ready!\n";
#endif
	scPacketReady packet;
	packet.size = sizeof( packet );
	packet.type = SC_READY;
	packet.id = who;
	SendPacket( id, &packet );
}

UxVoid Server::SendPacketUnReady( UxInt32 id, UxInt32 who )
{
#ifdef LOG_ON
	std::cout << "[" << id << "] send unready!\n";
#endif
	scPacketUnReady packet;
	packet.size = sizeof( packet );
	packet.type = SC_UN_READY;
	packet.id = who;
	SendPacket( id, &packet );
}

UxVoid Server::SendPacketLeaveRoom( UxInt32 id, UxInt32 who )
{
#ifdef LOG_ON
	std::cout << "[" << id << "] send leave room!\n";
#endif
	scPacketLeaveRoom packet;
	packet.size = sizeof( packet );
	packet.type = SC_LEAVE_ROOM;
	packet.id = who;
	SendPacket( id, &packet );
}

UxVoid Server::SendPacketGameStart( UxInt32 id )
{
#ifdef LOG_ON
	std::cout << "[" << id << "] send game start!\n";
#endif
	scPacketGameStart packet;
	packet.size = sizeof( packet );
	packet.type = SC_GAME_START;
	SendPacket( id, &packet );
}

//in game
UxVoid Server::SendPacketPosition( UxInt32 id, UxInt32 who, UxSingle x, UxSingle y )
{
	scPacketPosition packet;
	packet.size = sizeof( packet );
	packet.type = SC_POSITION;
	packet.id = who;
	packet.x = x;
	packet.y = y;
	SendPacket( id, &packet );
}

UxVoid Server::SendPacketRotation( UxInt32 id )//일단 보류
{

}

UxVoid Server::SendPacketAnimation( UxInt32 id, UxInt32 who, UxInt8 anim )
{
#ifdef LOG_ON
	std::cout << "[" << id << "] send animation!\n";
#endif
	scPacketAnimation packet;
	packet.size = sizeof( packet );
	packet.type = SC_ANIMATOIN;
	packet.id = who;
	packet.anim = anim;
	SendPacket( id, &packet );
}

UxVoid Server::SendPacketHit( UxInt32 id )//일단 보류
{

}

UxVoid Server::SendPacketDeductHeart( UxInt32 id, UxInt32 who, UxInt32 num )
{
#ifdef LOG_ON
	std::cout << "[" << id << "] send deduct heart!\n";
#endif
	scPacketDeductHeart packet;
	packet.size = sizeof( packet );
	packet.type = SC_DEDUCT_HEART;
	packet.id = who;
	packet.num = num;
	SendPacket( id, &packet );
}

UxVoid Server::SendPacketDie( UxInt32 id, UxInt32 who )
{
#ifdef LOG_ON
	std::cout << "[" << id << "] send die!\n";
#endif
	scPacketDie packet;
	packet.size = sizeof( packet );
	packet.type = SC_DIE;
	packet.id = who;
	SendPacket( id, &packet );
}

UxVoid Server::SendPacketGameOver( UxInt32 id, PTC_Winner* winner )
{
#ifdef LOG_ON
	std::cout << "[" << id << "] send game over!\n";
#endif
	scGameOver packet;
	packet.size = sizeof( packet );
	packet.type = SC_GAME_OVER;
	memcpy( &packet.winner, winner, sizeof( packet.winner ) );
	SendPacket( id, &packet );
}

UxVoid Server::SendPacketLeftTime( UxInt32 id, UxUInt8 left_time )
{
#ifdef LOG_ON
	std::cout << "[" << id << "] send tick!\n";
#endif
	scLeftTime packet;
	packet.size = sizeof( packet );
	packet.type = SC_LEFT_TIME;
	packet.leftTime = left_time;
	SendPacket( id, &packet );
}

SOCKADDR_IN Server::GetServerAddr()
{
	return m_ServerAddr;
}

UxBool Server::IsAvailableId( const std::string& name )
{
	for ( auto&& c : m_clients )
		if ( c->name == name )
		{
#ifdef LOG_ON
			std::cout <<"이름 이미 있음"<< c->name << ", " << name << std::endl;
#endif
			return false;
		}
	return true;
}

UxVoid Server::CreateClientsSlot()
{
	std::cout << " Initializing g_clients\n";
	for ( int user_id = 0; user_id < maxClient; ++user_id ) 
	{
		SOCKETINFO* player = new SOCKETINFO;
		player->recv_over.wsabuf[0].len = maxBuffer;
		player->recv_over.wsabuf[0].buf = player->recv_over.net_buf;

		ZeroMemory( player->packet_buf, maxBuffer );
		m_clients[user_id] = player;
	}
	std::cout << "Slot Initialization Finished\n";
}