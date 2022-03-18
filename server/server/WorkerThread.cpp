

#include "Protocol.h"
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
		if ( !bSuccessful ) 
		{
			UxInt32 err_no = GetLastError();
			std::cout << "Error!!" << std::endl;
		}

		SOCKET clientSock;
		if ( key == eventKey );
		else
		{
			if ( Server::GetInstance()->m_clients[key]->socket == nullptr )
			{
				std::cout << "nullptr Error!!" << std::endl;
				continue;
			}

			clientSock = Server::GetInstance()->m_clients[key]->socket->GetSocket();

			if ( num_byte == 0 )
			{
				// 클라이언트와 연결끊김 처리 필요
				std::cout << "recv 0 byte" << std::endl;
				DisconnectClient( key );
				continue;
			}
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
					unsigned char packet[maxBuffer];
					memcpy( packet, Server::GetInstance()->m_clients[key]->packet_buf, pr_size );
					memcpy( packet + pr_size, buf, psize - pr_size );
					ProcPacket( key, packet );

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
		else if ( EEventType::TICK == over_ex->ev_type )
		{
			message packet2msg;
			memset( &packet2msg, 0x00, sizeof( message ) );
			packet2msg.id = -1;
			packet2msg.roomNum = *( int* )over_ex->net_buf;
			UxInt8 buf[2];
			buf[0] = sizeof( buf );
			buf[1] = SC_LEFT_TIME;
			packet2msg.buff = buf;
			Server::GetInstance()->m_roomMsgQueue.push( packet2msg );
		}
		else
		{
			std::cout << "[LOG] Unknown Event Type!!\n";
		}
	}
}

UxVoid WorkerThread::JoinThread()
{
	mythread.join();
}


UxVoid WorkerThread::ProcPacket( UxInt32 id, UxVoid* buf )
{
	UxInt8* packet = reinterpret_cast< UxInt8* >( buf );
	message packet2msg;
	memset( &packet2msg, 0x00, sizeof( message ) );
	packet2msg.id = id;
	packet2msg.name = Server::GetInstance()->m_clients[id]->name;
	packet2msg.roomNum = Server::GetInstance()->m_clients[id]->roomNum;
	packet2msg.buff = buf;

	if ( packet[1] == CS_LOGIN )
	{
#ifdef LOG_ON
		std::cout << "[" << id << "] recv login packet" << std::endl;
#endif
		csPacketLogin* loginPacket = reinterpret_cast< csPacketLogin* >( packet );
		std::string str { loginPacket->id };
		if ( Server::GetInstance()->IsAvailableId( str ) )
		{
			Server::GetInstance()->m_clients[id]->name = str;
			Server::GetInstance()->SendPacketLoginOk( id );
		}
		else
		{
			Server::GetInstance()->SendPacketLoginDeny( id );
		}
	}
	else if ( packet[1] == CS_JOIN_GAME )
	{
#ifdef LOG_ON
		std::cout << "[" << id << "] recv join game packet" << std::endl;
#endif
		if ( Server::GetInstance()->m_clients[id]->name.length() > 0 )
			Server::GetInstance()->SendPacketJoinGameOk( id );
		else
			Server::GetInstance()->SendPacketJoinGameDeny( id );
	}
	else if ( packet[1] == CS_EXIT_GAME )
	{
#ifdef LOG_ON
		std::cout << "[" << id << "] recv exit game packet" << std::endl;
#endif
		DisconnectClient( id );
	}
	else if ( packet[1] == CS_ROOM_LIST )
	{
#ifdef LOG_ON
		//std::cout << "[" << id << "] recv room list packet" << std::endl;
#endif
		PTC_Room rooms[5];
		UxInt32 num = Server::GetInstance()->m_roomManager.m_rooms.size();
		if ( num > 0 )
		{
			UxInt32 count { 0 };
			for ( auto&& r : Server::GetInstance()->m_roomManager.m_rooms )
			{
				rooms[count].id = r.second->GetRoomNum();
				rooms[count].participant = r.second->GetcurPlayerNum();
				//std::cout << "방 이름 :" << r.second->GetRoomName() << std::endl;
				strcpy_s( rooms[count].name, r.second->GetRoomName().c_str() );

				++count;
				if ( count == 5 )
				{
					Server::GetInstance()->SendPacketRoomList( id, num, rooms );
					count = 0;
					ZeroMemory( &rooms, sizeof( rooms ) );
				}
			}
			if ( count % 5 != 0 )
			{
				for ( UxInt32 i = count % 5; i < 5; ++i )
					rooms[i].id = -1;
				Server::GetInstance()->SendPacketRoomList( id, num, rooms );
			}
		}
		else
		{
			for ( auto&& r : rooms )
				r.id = -1;
			Server::GetInstance()->SendPacketRoomList( id, num, rooms );
		}
	}
	else
	{
		Server::GetInstance()->m_roomMsgQueue.push( packet2msg );
	}
}

UxVoid WorkerThread::DisconnectClient( UxInt32 clientID )
{
	//방 퇴장 처리
	SOCKET clientSock = Server::GetInstance()->m_clients[clientID]->socket->GetSocket();
	UxInt32 roomNum = Server::GetInstance()->m_clients[clientID]->roomNum;

	closesocket( clientSock );
	ZeroMemory( Server::GetInstance()->m_clients[clientID], sizeof( SOCKETINFO ) );
	Server::GetInstance()->m_clients[clientID]->socket = nullptr;
	Server::GetInstance()->m_clients[clientID]->roomNum = notInRoom;
	Server::GetInstance()->m_clients[clientID]->name = "";
	Server::GetInstance()->m_clients[clientID]->isConnected = false;

	if ( roomNum != notInRoom )
	{
		csPacketLeaveRoom packet;
		packet.size = sizeof( packet );
		packet.type = CS_LEAVE_ROOM;
		message msg { clientID, "",roomNum, &packet };
		Server::GetInstance()->m_roomMsgQueue.push( msg );
	}

	std::cout << "Client ID - " << clientID << " disconnected.\n";
}
