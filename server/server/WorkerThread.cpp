

#include "Protocol.h"
#include "Server.h"
#include "Slander.h"
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
			std::cout << "GQCS Error : " << err_no << std::endl;
			DisconnectClient( key );
			continue;
		}

		SOCKET clientSock;
		if ( key == eventKey );
		else
		{
			if ( Server::GetInstance()->m_clients[key]->socket == nullptr )
			{
				std::cout << "[Error] socket nullptr" << std::endl;
				continue;
			}
			if ( num_byte == 0 )
			{
				// 클라이언트와 연결끊김 처리 필요
				std::cout << "[Log] recv 0 byte" << std::endl;
				DisconnectClient( key );
				continue;
			}

			clientSock = Server::GetInstance()->m_clients[key]->socket->GetSocket();
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
				if ( 0 == psize ) psize = buf[0];
				if ( num_byte + pr_size >= psize )
				{
					unsigned char packet[maxBuffer];
					//char* packet = new char[maxBuffer];
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
					std::cout << "[Error] recv\n";
				}
			}
		}
		else if ( EEventType::SEND == over_ex->ev_type )
		{
			delete over_ex;
		}
		//밑으로 동일한거 처리 필요
		else if ( EEventType::TICK == over_ex->ev_type )
		{
			message packet2msg;
			memset( &packet2msg, 0x00, sizeof( message ) );
			packet2msg.id = key; //eventKey
			packet2msg.roomNum = ( *( EVENTINFO* )over_ex->net_buf ).roomNum;
			memcpy( packet2msg.buff, over_ex->net_buf, sizeof( EVENTINFO ) );
			Server::GetInstance()->m_roomMsgQueue.push( packet2msg );
		}
		else if ( EEventType::INVINCIBLEDONE == over_ex->ev_type )
		{
			message packet2msg;
			memset( &packet2msg, 0x00, sizeof( message ) );
			packet2msg.id = key; //eventKey
			packet2msg.roomNum = ( *( EVENTINFO* )over_ex->net_buf ).roomNum;
			memcpy( packet2msg.buff, over_ex->net_buf, sizeof( EVENTINFO ) );
			Server::GetInstance()->m_roomMsgQueue.push( packet2msg );
		}
		else if ( EEventType::RESETCOOLTIME == over_ex->ev_type )
		{
			message packet2msg;
			memset( &packet2msg, 0x00, sizeof( message ) );
			packet2msg.id = key; //eventKey
			packet2msg.roomNum = ( *( EVENTINFO* )over_ex->net_buf ).roomNum;
			memcpy( packet2msg.buff, over_ex->net_buf, sizeof( EVENTINFO ) );
			Server::GetInstance()->m_roomMsgQueue.push( packet2msg );
		}
		else//그냥 여기서 이벤트들 모두 처리
		{
			message packet2msg;
			memset( &packet2msg, 0x00, sizeof( message ) );
			packet2msg.id = key; //eventKey
			packet2msg.roomNum = ( *( EVENTINFO* )over_ex->net_buf ).roomNum;
			memcpy( packet2msg.buff, over_ex->net_buf, sizeof( EVENTINFO ) );
			Server::GetInstance()->m_roomMsgQueue.push( packet2msg );
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
	if ( packet[1] == CS_LOGIN )
	{
#ifdef LOG_ON
		std::cout << "[" << id << "] recv login packet" << std::endl;
#endif
		csPacketLogin* loginPacket = reinterpret_cast< csPacketLogin* >( packet );		
		
		std::wstring strUTF { loginPacket->id };
		std::wcout << strUTF << std::endl;

		if ( Server::GetInstance()->IsAvailableId( strUTF ) && !Slander::IsSlander( strUTF ) )
		{
			Server::GetInstance()->m_clients[id]->name = strUTF;
			Server::GetInstance()->SendPacketLoginOk( id, strUTF );
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
		std::cout << "[" << id << "] recv room list packet" << std::endl;
#endif
		Server::GetInstance()->SendPacketRoomList( id, -1, L"", -1 );
		for ( auto&& room : Server::GetInstance()->m_roomManager.m_rooms )
		{
			Server::GetInstance()->SendPacketRoomList( id, room.second->GetRoomNum(), room.second->GetRoomName(), room.second->GetcurPlayerNum() );
		}
	}
	else
	{
		message packet2msg;
		memset( &packet2msg, 0x00, sizeof( message ) );
		packet2msg.id = id;
		packet2msg.name = Server::GetInstance()->m_clients[id]->name;
		wchar_t tmp[18];
		wcscpy_s( tmp, Server::GetInstance()->m_clients[id]->name.c_str() );
		packet2msg.name.assign( tmp, Server::GetInstance()->m_clients[id]->name.length() );
		packet2msg.roomNum = Server::GetInstance()->m_clients[id]->roomNum;
		memcpy( packet2msg.buff, buf, packet[0] );
		Server::GetInstance()->m_roomMsgQueue.push( packet2msg );
	}
}

UxVoid WorkerThread::DisconnectClient( UxInt32 clientID )
{
	//방 퇴장 처리
	if ( Server::GetInstance()->m_clients[clientID]->socket != nullptr )
	{
		SOCKET clientSock = Server::GetInstance()->m_clients[clientID]->socket->GetSocket();
		closesocket( clientSock );
		std::cout << "[" << clientID << "] close socket\n";
	}
	UxInt32 roomNum = Server::GetInstance()->m_clients[clientID]->roomNum;

	Server::GetInstance()->m_clients[clientID]->socket = nullptr;
	Server::GetInstance()->m_clients[clientID]->roomNum = notInRoom;
	Server::GetInstance()->m_clients[clientID]->name = L"";
	Server::GetInstance()->m_clients[clientID]->isConnected = false;
	ZeroMemory( Server::GetInstance()->m_clients[clientID], sizeof( SOCKETINFO ) );

	if ( roomNum != notInRoom )
	{
		csPacketLeaveRoom packet;
		packet.size = sizeof( packet );
		packet.type = CS_LEAVE_ROOM;
		message msg;
		msg.id = clientID;
		msg.name = L"";
		msg.roomNum = roomNum;
		memcpy( msg.buff, &packet, packet.size );
		Server::GetInstance()->m_roomMsgQueue.push( msg );
	}

	std::cout << "[" << clientID << "] disconnected.\n";
}
