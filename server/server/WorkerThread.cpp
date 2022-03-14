

#include "Header.h"
#include "Global.h"
#include "Server.h"
#include "WorkerThread.h"


UxVoid WorkerThread::InitThread()
{
	m_ServerAddr = Server::GetInstance()->GetServerAddr();
	m_addrLen = sizeof( m_ServerAddr );

	mythread = std::thread( [&] () {WorkerThread::ProcThread(); } );
}

UxVoid WorkerThread::ProcThread()
{
	while ( true ) {
		DWORD num_byte;
		ULONG key;
		PULONG p_key = &key;
		WSAOVERLAPPED* p_over;

		BOOL bSuccessful = GetQueuedCompletionStatus( Server::GetInstance()->m_iocp, &num_byte, ( PULONG_PTR )p_key, &p_over, INFINITE );
		if ( !bSuccessful ) {
			UxInt32 err_no = GetLastError();
			std::cout << "Error!!" << std::endl;
		}

		SOCKET clientSock;
		if ( key == eventKey );
		if ( Server::GetInstance()->m_clients[key]->socket == nullptr ) {
			std::cout << "nullptr Error!!" << std::endl;
			continue;
		}

		clientSock = Server::GetInstance()->m_clients[key]->socket->GetSocket();

		if ( num_byte == 0 )
		{
			// 클라이언트와 연결끊김.
			DisconnectClient( key, clientSock );
			continue;
		}


		OVER_EX* over_ex = reinterpret_cast< OVER_EX* >( p_over );

		if ( EEventType::RECV == over_ex->ev_type )
		{
			char* buf = Server::GetInstance()->m_clients[key]->recv_over.net_buf;
			unsigned psize = Server::GetInstance()->m_clients[key]->curr_packet_size;
			unsigned pr_size = Server::GetInstance()->m_clients[key]->prev_packet_data;
			message msg { -1 };
			while ( num_byte > 0 )
			{
				if ( 0 == psize ) psize = ( BYTE )buf[0];
				if ( num_byte + pr_size >= psize )
				{
					// 지금 패킷 완성 가능
					unsigned char packet[maxBuffer];
					memcpy( packet, Server::GetInstance()->m_clients[key]->packet_buf, pr_size );
					memcpy( packet + pr_size, buf, psize - pr_size );
					msg = ProcPacket( key, packet );

					/*방에있음 처리 여기서?
					if ( msg.type != NO_MSG ) {
						int room_ID = SHARED_RESOURCE::g_clients[key]->room_id;
						SHARED_RESOURCE::g_rooms[room_ID]->PushMsg( msg );
					}
					*/

					num_byte -= psize - pr_size;
					buf += psize - pr_size;
					psize = 0; pr_size = 0;
				}
				else
				{
					//락프리 필요....
					memcpy( Server::GetInstance()->m_clients[key]->packet_buf + pr_size, buf, num_byte );
					pr_size += num_byte;
					num_byte = 0;
				}
			}
			Server::GetInstance()->m_clients[key]->curr_packet_size = psize;
			Server::GetInstance()->m_clients[key]->prev_packet_data = pr_size;

			DWORD flags = 0;
			memset( &over_ex->over, 0x00, sizeof( WSAOVERLAPPED ) );
			memset( &msg, 0, sizeof( message ) );
			over_ex->ev_type = EEventType::RECV;

			int retval = WSARecv( clientSock, over_ex->wsabuf, 1, 0, &flags, &over_ex->over, 0 );
			if ( retval == SOCKET_ERROR ) {
				if ( WSAGetLastError() != ERROR_IO_PENDING )
				{
					std::cout << "Recv Error!!!! \n";
				}
			}
		}
		else if ( EEventType::SEND == over_ex->ev_type )
		{
			delete over_ex;
		}

		else
		{
			std::cout << "[LOG] Unknown Event Type!!\n";
		}
	}
}

void WorkerThread::JoinThread()
{
	mythread.join();
}


message WorkerThread::ProcPacket( int id, void* buf )
{
	char* packet = reinterpret_cast< char* >( buf );
	message packet2msg;
	memset( &packet2msg, 0x00, sizeof( message ) );
	packet2msg.id = id;
	packet2msg.type = noMsg;

	switch ( packet[1] )
	{
	// 패킷 종류따라 처리
	default:
		std::cout << "[LOG] Invalid Packet Type Error!\n";
		while ( true );
	}
	return packet2msg;
}

UxVoid WorkerThread::DisconnectClient( int clientID, SOCKET client )
{
	//방 퇴장 처리

	// 클라이언트와 연결 종료. 락프리 필요....
	ZeroMemory( Server::GetInstance()->m_clients[clientID], sizeof( SOCKETINFO ) );

	Server::GetInstance()->m_clients[clientID]->socket = nullptr;
	//closesocket( client );
	std::cout << "Client ID - " << clientID << " disconnected.\n";
}