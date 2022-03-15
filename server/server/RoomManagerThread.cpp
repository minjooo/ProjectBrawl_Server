

#include "Header.h"
#include "Protocol.h"
#include "Server.h"
#include "RoomManagerThread.h"


UxVoid RoomManagerThread::InitThread()
{
	mythread = std::thread( [&] () {RoomManagerThread::ProcThread(); } );
}

UxVoid RoomManagerThread::ProcThread()
{
	while ( true )
	{
		while ( true == Server::GetInstance()->m_roomMsgQueue.empty() )
		{
			std::this_thread::sleep_for( std::chrono::milliseconds( 10 ) );
		}
		std::queue<message> copiedQ = Server::GetInstance()->m_roomMsgQueue;
		while ( !Server::GetInstance()->m_roomMsgQueue.empty() )
			Server::GetInstance()->m_roomMsgQueue.pop();

		while ( !copiedQ.empty() )
		{
			const message& msg = copiedQ.front();
			copiedQ.pop();

			char* packet = reinterpret_cast< char* >( msg.buff );

			switch ( packet[1] )
			{
			case CS_MAKE_ROOM:
			{
				csPacketMakeRoom* tmpPacket = reinterpret_cast< csPacketMakeRoom* >( packet );
				std::string tmpRoomName { tmpPacket->name };
				UxInt32 num = Server::GetInstance()->m_roomManager.AddNewRoom( tmpRoomName );
				Server::GetInstance()->m_roomManager.m_rooms[num]->EnterRoom( msg.id, msg.name );
				//send 필요
			}
			break;
			case CS_JOIN_ROOM:
			{
				csPacketJoinRoom* tmpPacket = reinterpret_cast< csPacketJoinRoom* >( packet );
				Server::GetInstance()->m_roomManager.m_rooms[tmpPacket->roomNum]->EnterRoom( msg.id, msg.name );
				//send 필요
			}
			break;
			case CS_SELECT_CHARACTER:
			{
				Server::GetInstance()->m_roomManager.m_rooms[msg.roomNum]->PushMsg( packet );
			}
			break;
			case CS_READY:
			{
				Server::GetInstance()->m_roomManager.m_rooms[msg.roomNum]->PushMsg( packet );
			}
			break;
			case CS_UN_READY:
			{
				Server::GetInstance()->m_roomManager.m_rooms[msg.roomNum]->PushMsg( packet );
			}
			break;
			case CS_LEAVE_ROOM:
			{
				csPacketLeaveRoom* tmpPacket = reinterpret_cast< csPacketLeaveRoom* >( packet );
				Server::GetInstance()->m_roomManager.m_rooms[msg.roomNum]->LeaveRoom( msg.id );
				//방 비었음 처리 필요
				//send필요
			}
			break;
			case CS_POSITION:
			{
				Server::GetInstance()->m_roomManager.m_rooms[msg.roomNum]->PushMsg( packet );
			}
			break;
			case CS_ROTATE:
			{
				Server::GetInstance()->m_roomManager.m_rooms[msg.roomNum]->PushMsg( packet );
			}
			break;
			case CS_ANIMATION:
			{
				Server::GetInstance()->m_roomManager.m_rooms[msg.roomNum]->PushMsg( packet );
			}
			break;
			case CS_ATTACK:
			{
				Server::GetInstance()->m_roomManager.m_rooms[msg.roomNum]->PushMsg( packet );
			}
			break;
			case CS_HEART_DECREAS:
			{
				Server::GetInstance()->m_roomManager.m_rooms[msg.roomNum]->PushMsg( packet );
			}
			break;
			case CS_DIE:
			{
				Server::GetInstance()->m_roomManager.m_rooms[msg.roomNum]->PushMsg( packet );
			}
			break;
			default:
				break;
			}
		}

		//update
		Server::GetInstance()->m_roomManager.UpdateRooms();
	}
}

UxVoid RoomManagerThread::JoinThread()
{
	mythread.join();
}