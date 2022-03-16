

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
			std::this_thread::sleep_for( std::chrono::milliseconds( 5 ) );
		}

		while ( !Server::GetInstance()->m_roomMsgQueue.empty() )
		{
			message msg;
			if ( true == Server::GetInstance()->m_roomMsgQueue.try_pop( msg ) )
			{
				char* packet = reinterpret_cast< char* >( msg.buff );

				if ( packet[1] == CS_MAKE_ROOM )
				{
					std::cout << "[" << msg.id << "] recv make room packet" << std::endl;
					csPacketMakeRoom* tmpPacket = reinterpret_cast< csPacketMakeRoom* >( packet );
					std::string tmpRoomName { tmpPacket->name };
					UxInt32 num = Server::GetInstance()->m_roomManager.AddNewRoom( tmpRoomName );
					message tmpMsg = msg;
					csPacketJoinRoom p;
					p.size = sizeof( csPacketJoinRoom );
					p.type = CS_JOIN_ROOM;
					p.roomNum = num;
					tmpMsg.buff = &p;
					Server::GetInstance()->m_roomManager.m_rooms[msg.roomNum]->PushMsg( tmpMsg );
				}
				else
				{
					Server::GetInstance()->m_roomManager.m_rooms[msg.roomNum]->PushMsg( msg );
				}
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