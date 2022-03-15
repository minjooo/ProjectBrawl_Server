

#include "Protocol.h"
#include "Header.h"
#include "TypeDef.h"
#include "Server.h"
#include "Room.h"


Room::Room( UxInt32 room_num )
{
	Initialize( room_num );
}

Room::Room( UxInt32 room_num, std::string room_name )
{
	Initialize( room_num );
	m_roomName = room_name;
}

Room::~Room()
{
	for ( auto& player : m_players )
	{
		if ( player != nullptr )
		{
			delete player;
			player = nullptr;
		}
	}
}

void Room::Initialize( UxInt32 room_num )
{
	m_roomNum = room_num;
	m_curPlayerNum = 0;
	m_isGameStarted = false;

	for ( auto& player : m_players )
		player = new Player();
}

UxVoid Room::Update()
{
	if ( this == nullptr )
		return;

	while ( !m_roomMsg.empty() )
	{
		message msg = m_roomMsg.front();
		m_roomMsg.pop();

		char* packet = reinterpret_cast< char* >( msg.buff );

		switch ( packet[1] )
		{
		case CS_JOIN_ROOM:
		{
			csPacketJoinRoom* tmpPacket = reinterpret_cast< csPacketJoinRoom* >( packet );
			UxBool res = EnterRoom( msg.id, msg.name );
			if ( res )
			{
				for ( auto&& p : m_players )
					if ( !p->IsEmpty() )
						Server::GetInstance()->SendPacketJoinRoomOk( p->GetId(), msg.id );
			}
			else
			{
				Server::GetInstance()->SendPacketJoinRoomDeny( msg.id );
			}
		}
		break;
		case CS_LEAVE_ROOM:
		{
			csPacketLeaveRoom* tmpPacket = reinterpret_cast< csPacketLeaveRoom* >( packet );
			LeaveRoom( msg.id );
			for ( auto&& p : m_players )
				if ( !p->IsEmpty() )
					Server::GetInstance()->SendPacketLeaveRoom( p->GetId(), msg.id );
		}
		break;
		case CS_SELECT_CHARACTER:
		{
			csPacketSelectCharacter* tmpPacket = reinterpret_cast< csPacketSelectCharacter* >( packet );
			m_players[msg.id]->SetCharacter( tmpPacket->character );
			for ( auto&& p : m_players )
				if ( !p->IsEmpty() && p->GetId() != msg.id )
					Server::GetInstance()->SendPacketSelectCharacter( p->GetId(), msg.id, tmpPacket->character );
		}
		break;
		case CS_READY:
		{
			m_players[msg.id]->SetReady( true );
			for ( auto&& p : m_players )
				if ( !p->IsEmpty() && p->GetId() != msg.id )
					Server::GetInstance()->SendPacketReady( p->GetId(), msg.id );
			
			//모두 준비됐으면 게임 시작
			if ( IsGameStartAble() )
				for ( auto&& p : m_players )
					Server::GetInstance()->SendPacketReady( p->GetId(), msg.id );
		}
		break;
		case CS_UN_READY:
		{
			m_players[msg.id]->SetReady( false );
			for ( auto&& p : m_players )
				if ( !p->IsEmpty() && p->GetId() != msg.id )
					Server::GetInstance()->SendPacketUnReady( p->GetId(), msg.id );
		}
		break;
		case CS_POSITION:
		{
			csPacketPosition* tmpPacket = reinterpret_cast< csPacketPosition* >( packet );
			m_players[msg.id]->SetPos( tmpPacket->x, tmpPacket->y );
		}
		break;
		case CS_ROTATE:
		{
			//나중에 결정되면 추가 필요
		}
		break;
		case CS_ANIMATION:
		{
			csPacketAnimation* tmpPacket = reinterpret_cast< csPacketAnimation* >( packet );
			m_players[msg.id]->SetAnim( tmpPacket->anim );
			for ( auto&& p : m_players )
				if ( !p->IsEmpty() && p->GetId() != msg.id )
					Server::GetInstance()->SendPacketAnimation( p->GetId(), msg.id, tmpPacket->anim );
		}
		break;
		case CS_ATTACK:
		{
			//나중에 추가 필요
		}
		break;
		case CS_DEDUCT_HEART:
		{
			m_players[msg.id]->DeductHeart();
			for ( auto&& p : m_players )
				if ( !p->IsEmpty() )
					Server::GetInstance()->SendPacketDeductHeart( p->GetId(), msg.id, p->GetHeartNum() );
			//죽었나 봐야하나?
		}
		break;
		case CS_DIE:
		{
			m_players[msg.id]->SetDie();
			for ( auto&& p : m_players )
				if ( !p->IsEmpty() && p->GetId() != msg.id )
					Server::GetInstance()->SendPacketDie( p->GetId(), msg.id );
		}
		break;
		default:
			std::cout << "\n\nInvalid Packet Type Error!\n\n";
			break;
		}
	}

	//in game send (pos, rot, attack)
	for ( auto&& p1 : m_players )
		for ( auto&& p2 : m_players )
			if ( !p1->IsEmpty() && !p2->IsEmpty() && p1 != p2 )
				Server::GetInstance()->SendPacketPosition( p1->GetId(), p2->GetId(), p2->GetPosX(), p2->GetPosY() );
}

UxVoid Room::PushMsg( const message& msg )
{
	m_roomMsg.push( msg );
}

UxBool Room::IsEmpty()
{
	if ( m_curPlayerNum <= 0 )
		return true;
	return false;
}

UxBool Room::IsGameStarted()
{
	return m_isGameStarted;
}

UxBool Room::IsGameStartAble()
{
	for ( auto&& p : m_players )
		if ( p->IsEmpty() )return false;
	for ( auto&& p : m_players )
		if ( !p->GetReady() )return false;
	return true;
}

UxBool Room::EnterRoom( UxInt32 id, std::string name )
{
	if ( m_curPlayerNum >= maxPlayer )
		return false;

	for ( auto&& player : m_players )
	{
		if ( player->IsEmpty() )
		{
			player->EnterRoom( id, name );
			++m_curPlayerNum;

			Server::GetInstance()->m_clients[id]->roomNum = m_roomNum;
			return true;
		}
	}

	return false;
}

UxVoid Room::LeaveRoom( UxInt32 id )
{
	for ( auto&& player : m_players )
	{
		if ( player->GetId() == id )
		{
			player->Reset();
			--m_curPlayerNum;

			Server::GetInstance()->m_clients[id]->roomNum = notInRoom;
			break;
		}
	}
}

UxVoid Room::GameStart()
{
	//모든 플레이어에게 게임 시작 패킷 보내주기

	m_isGameStarted = true;
}

UxVoid Room::GameOver( UxInt32 winner )
{
	m_isGameStarted = false;

	//모든 플레이어에게 위너 알려주기
}

std::string Room::GetRoomName()
{
	return m_roomName;
}

UxInt32 Room::GetcurPlayerNum()
{
	return m_curPlayerNum;
}

UxInt32 Room::GetRoomNum()
{
	return m_roomNum;
}
