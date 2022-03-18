

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
		//while ( true == Server::GetInstance()->m_roomMsgQueue.empty() )
		//{
		//	//std::this_thread::sleep_for( std::chrono::milliseconds( 1 ) );
		//}

		while ( !Server::GetInstance()->m_roomMsgQueue.empty() )
		{
			message msg;
			if ( true == Server::GetInstance()->m_roomMsgQueue.try_pop( msg ) )
			{
				if ( msg.id == eventKey )
				{
					if ( Server::GetInstance()->m_roomManager.m_rooms.count( msg.roomNum ) > 0 )
						Server::GetInstance()->m_roomManager.m_rooms[msg.roomNum]->PushMsg( msg );
					else
						std::cout << "invalid room num\n";
				}
				else
				{
					char* packet = reinterpret_cast< char* >( msg.buff );

					if ( packet[1] == CS_MAKE_ROOM )
					{
#ifdef LOG_ON
						std::cout << "[" << msg.id << "] recv make room packet" << std::endl;
#endif
						csPacketMakeRoom* tmpPacket = reinterpret_cast< csPacketMakeRoom* >( packet );
						std::string tmpRoomName { tmpPacket->name };
						UxInt32 num = Server::GetInstance()->m_roomManager.AddNewRoom( tmpRoomName );
						Server::GetInstance()->SendPacketMakeRoomOk( msg.id, num );
						message tmpMsg = msg;
						csPacketJoinRoom p;
						p.size = sizeof( csPacketJoinRoom );
						p.type = CS_JOIN_ROOM;
						p.roomNum = num;
						tmpMsg.buff = &p;
						Server::GetInstance()->m_roomManager.m_rooms[num]->PushMsg( tmpMsg );
					}
					else if ( packet[1] == CS_JOIN_ROOM )
					{
						csPacketJoinRoom* tmpPacket = reinterpret_cast< csPacketJoinRoom* >( packet );
						if ( Server::GetInstance()->m_roomManager.m_rooms.count( tmpPacket->roomNum ) > 0 )
							Server::GetInstance()->m_roomManager.m_rooms[tmpPacket->roomNum]->PushMsg( msg );
						else
							std::cout << "invalid room num (join room)\n";
					}
					else
					{
						if ( Server::GetInstance()->m_roomManager.m_rooms.count( msg.roomNum ) > 0 )
							Server::GetInstance()->m_roomManager.m_rooms[msg.roomNum]->PushMsg( msg );
						else
							std::cout << "invalid room num\n";
					}
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