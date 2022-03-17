

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
	m_leftTime = playTime;

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
#ifdef LOG_ON
			std::cout << "[" << msg.id << "] recv join room packet" << std::endl;
#endif
			csPacketJoinRoom* tmpPacket = reinterpret_cast< csPacketJoinRoom* >( packet );
			UxBool res = EnterRoom( msg.id, msg.name );
			if ( res )
			{
				for ( auto&& p : m_players )
					if ( !p->IsEmpty() )
						Server::GetInstance()->SendPacketJoinRoomOk( p->GetId(), msg.id, msg.name );
			}
			else
			{
				Server::GetInstance()->SendPacketJoinRoomDeny( msg.id );
			}
		}
		break;
		case CS_LEAVE_ROOM:
		{
#ifdef LOG_ON
			std::cout << "[" << msg.id << "] recv leave room packet" << std::endl;
#endif
			csPacketLeaveRoom* tmpPacket = reinterpret_cast< csPacketLeaveRoom* >( packet );
			LeaveRoom( msg.id );
			for ( auto&& p : m_players )
				if ( !p->IsEmpty() )
					Server::GetInstance()->SendPacketLeaveRoom( p->GetId(), msg.id );
		}
		break;
		case CS_SELECT_CHARACTER:
		{
#ifdef LOG_ON
			std::cout << "[" << msg.id << "] recv select character packet" << std::endl;
#endif
			csPacketSelectCharacter* tmpPacket = reinterpret_cast< csPacketSelectCharacter* >( packet );
			m_players[m_id2index[msg.id]]->SetCharacter( tmpPacket->character );
			for ( auto&& p : m_players )
				if ( !p->IsEmpty() && p->GetId() != msg.id )
					Server::GetInstance()->SendPacketSelectCharacter( p->GetId(), msg.id, tmpPacket->character );
		}
		break;
		case CS_READY:
		{
#ifdef LOG_ON
			std::cout << "[" << msg.id << "] recv ready packet" << std::endl;
#endif
			m_players[m_id2index[msg.id]]->SetReady( true );
			for ( auto&& p : m_players )
				if ( !p->IsEmpty() && p->GetId() != msg.id )
					Server::GetInstance()->SendPacketReady( p->GetId(), msg.id );

			//모두 준비됐으면 게임 시작
			if ( IsGameStartAble() )
			{
				EVENT ev { eventKey,m_roomNum, std::chrono::high_resolution_clock::now() + std::chrono::seconds( 1 )  , EEventType::TICK };
				Server::GetInstance()->m_timerQueue.push( ev );
				for ( auto&& p : m_players )
					Server::GetInstance()->SendPacketGameStart( p->GetId() );
			}
		}
		break;
		case CS_UN_READY:
		{
#ifdef LOG_ON
			std::cout << "[" << msg.id << "] recv unready packet" << std::endl;
#endif
			m_players[m_id2index[msg.id]]->SetReady( false );
			for ( auto&& p : m_players )
				if ( !p->IsEmpty() && p->GetId() != msg.id )
					Server::GetInstance()->SendPacketUnReady( p->GetId(), msg.id );
		}
		break;
		case CS_POSITION:
		{
#ifdef LOG_ON
			std::cout << ".";
#endif
			csPacketPosition* tmpPacket = reinterpret_cast< csPacketPosition* >( packet );
			m_players[m_id2index[msg.id]]->SetPos( tmpPacket->x, tmpPacket->y );
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
			m_players[m_id2index[msg.id]]->SetAnim( tmpPacket->anim );
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
#ifdef LOG_ON
			std::cout << "[" << msg.id << "] recv deduct heart packet" << std::endl;
#endif
			m_players[m_id2index[msg.id]]->DeductHeart();
			for ( auto&& p : m_players )
				if ( !p->IsEmpty() )
					Server::GetInstance()->SendPacketDeductHeart( p->GetId(), msg.id, p->GetHeartNum() );
			//죽었나 봐야하나?
		}
		break;
		case CS_DIE:
		{
#ifdef LOG_ON
			std::cout << "[" << msg.id << "] recv die packet" << std::endl;
#endif
			m_players[m_id2index[msg.id]]->SetDie();
			for ( auto&& p : m_players )
				if ( !p->IsEmpty() && p->GetId() != msg.id )
					Server::GetInstance()->SendPacketDie( p->GetId(), msg.id );
		}
		break;
		case SC_LEFT_TIME:
		{
			--m_leftTime;
#ifdef LOG_ON
			std::cout << "TICK!" << std::endl;
#endif
			if ( m_leftTime == 0 )
			{
				GameOver();
			}
			else if ( m_leftTime > 0 && m_isGameStarted )
			{
				EVENT ev { eventKey,m_roomNum, std::chrono::high_resolution_clock::now() + std::chrono::seconds( 1 )  , EEventType::TICK };
				Server::GetInstance()->m_timerQueue.push( ev );
				for ( auto&& p : m_players )
					if ( !p->IsEmpty() )
						Server::GetInstance()->SendPacketLeftTime( p->GetId(), m_leftTime );
			}
		}
		break;
		default:
		{
			std::cout << "Invalid Packet Type Error! : " << (int)( packet[1] ) << "\n";
		}
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

	for ( int i = 0; i < maxPlayer; ++i )
	{
		if ( m_players[i]->IsEmpty() )
		{
			m_players[i]->EnterRoom( id, name );
			m_id2index.insert( std::make_pair( id, i ) );
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
			m_id2index.erase( id );
			player->Reset();
			--m_curPlayerNum;

			Server::GetInstance()->m_clients[id]->roomNum = notInRoom;
			break;
		}
	}
}

UxVoid Room::GameStart()
{
	m_isGameStarted = true;
}

UxVoid Room::GameOver()
{
	m_isGameStarted = false;
	PTC_Winner winner[4];
	int i = 0;
	for ( auto&& p : m_players )
	{
		winner[i].id = p->GetId();
		if ( p->IsEmpty() || !p->IsAlive() )
			winner->isWin = 0;
		else
			winner->isWin = 1;
		++i;
	}

	for ( auto&& p : m_players )
		if ( !p->IsEmpty() )
			Server::GetInstance()->SendPacketGameOver( p->GetId(), winner );
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
