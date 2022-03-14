
#include "Header.h"
#include "Protocol.h"
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
	//room초기화 필요

	//client 생성
	CreateClientsSlot();

	std::wcout.imbue( std::locale( "korean" ) );


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

		SOCKETINFO* new_player = new SOCKETINFO;
		new_player->socket = clientSocket;

		new_player->recv_over.wsabuf[0].len = maxBuffer;
		new_player->recv_over.wsabuf[0].buf = new_player->recv_over.net_buf;

		new_player->recv_over.ev_type = EEventType::RECV;

		new_player->curr_packet_size = 0;
		new_player->prev_packet_data = 0;

		new_player->isConnected = true;

		m_clients[user_id] = new_player;
		std::cout << user_id << " accept!" << std::endl;

		CreateIoCompletionPort( reinterpret_cast< HANDLE >( clientSocket->GetSocket() ), m_iocp, user_id, 0 );

		memset( &m_clients[user_id]->recv_over.over, 0, sizeof( WSAOVERLAPPED ) );
		clientSocket->WSAReceive( m_clients[user_id]->recv_over.wsabuf, 1, NULL, &flags, &m_clients[user_id]->recv_over.over );
		clientSocket = nullptr;
	}
}

SOCKADDR_IN Server::GetServerAddr()
{
	return m_ServerAddr;
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