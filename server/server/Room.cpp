

#include "Protocol.h"
#include "Header.h"
#include "TypeDef.h"
#include "Server.h"
#include "Room.h"


Room::Room( UxInt32 room_num )
{
	Initialize( room_num );
}

Room::Room( UxInt32 room_num, std::wstring room_name )
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
	m_destroy = false;
	m_roomNum = room_num;
	m_curPlayerNum = 0;
	m_isGameStarted = false;
	m_leftTime = playTime;

	for ( int i = 0; i < maxPlayer; ++i )
	{
		m_players[i] = new Player();
		m_players[i]->SetPos( startPosition[i].x, startPosition[i].y );
	}
}

UxVoid Room::Update()
{
	if ( this == nullptr )
		return;

	while ( !m_roomMsg.empty() )
	{
		message msg = m_roomMsg.front();
		m_roomMsg.pop();
		if ( msg.id == eventKey )
		{
			EVENTINFO* eventInfo = reinterpret_cast< EVENTINFO* >( msg.buff );
			switch ( eventInfo->event_type )
			{
			case EEventType::TICK:
			{
				--m_leftTime;
				if ( m_leftTime == 0 )
				{
					GameOver();
				}
				else if ( m_leftTime > 0 && m_isGameStarted )
				{
					EVENT ev { eventKey, *eventInfo, std::chrono::high_resolution_clock::now() + std::chrono::seconds( 1 )  , EEventType::TICK };
					Server::GetInstance()->m_timerQueue.push( ev );
					for ( auto&& p : m_players )
						if ( !p->IsEmpty() )
							Server::GetInstance()->SendPacketLeftTime( p->GetId(), m_leftTime );
				}
			}
			break;
			case EEventType::INVINCIBLEDONE:
			{
				m_players[m_id2index[eventInfo->id]]->SetInvincible( false );
				//클라에게 알림 필요?
			}
			break;
			case EEventType::RESETCOOLTIME:
			{
				m_players[m_id2index[eventInfo->id]]->SetSkillCool( true );
				Server::GetInstance()->SendPacketResetCoolTime( eventInfo->id );
			}
			break;
			case EEventType::SKILLDELAY:
			{
				if ( m_players[m_id2index[eventInfo->id]]->IsSkillCoolReset() )
				{
					m_players[m_id2index[eventInfo->id]]->SetSkillCool( false );
					EVENTINFO ei { eventInfo->id, m_roomNum,EEventType::RESETCOOLTIME };
					EVENT ev { eventKey, ei, std::chrono::high_resolution_clock::now() + std::chrono::seconds( coolTime )  , EEventType::RESETCOOLTIME };
					Server::GetInstance()->m_timerQueue.push( ev );

					for ( auto&& p : m_players )
						if ( !p->IsEmpty() && p->GetId() != eventInfo->id )
							if ( IsHit( m_id2index[eventInfo->id], m_id2index[p->GetId()] ) )
								Server::GetInstance()->SendPacketHit( p->GetId(), eventInfo->id, m_players[m_id2index[eventInfo->id]]->GetCharacterType(), hitType[m_players[m_id2index[eventInfo->id]]->GetCharacterType()] );
				}
			}
			break;
			default:
				break;
			}
		}
		else
		{
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
					Server::GetInstance()->SendPacketJoinRoomOk( m_players[m_id2index[msg.id]]->GetId(), m_roomName );
					//기존 유저에게 신규 유저 알림
					for ( auto&& p : m_players )
						if ( !p->IsEmpty() && p->GetId() != msg.id )
							Server::GetInstance()->SendPacketRoomUserList( p->GetId(), msg.id, msg.name, m_players[m_id2index[msg.id]]->GetCharacterType(), m_players[m_id2index[msg.id]]->GetReady() );
					//신규 유저에게 기존유저 알림
					for ( auto&& p : m_players )
						if ( !p->IsEmpty() && p->GetId() != msg.id )
							Server::GetInstance()->SendPacketRoomUserList( msg.id, p->GetId(), p->GetName(), p->GetCharacterType(), p->GetReady() );

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
				for ( auto&& p : m_players )
					if ( !p->IsEmpty() )
						Server::GetInstance()->SendPacketLeaveRoom( p->GetId(), msg.id );
				//문제 있을 수 있음
				LeaveRoom( msg.id );

				//플레이어가 한명 남았으면 game over 처리
				if ( IsGameOverAble() )
					GameOver();
				if ( m_curPlayerNum <= 0 )
					m_destroy = true;
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
					if ( !p->IsEmpty() )
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
					if ( !p->IsEmpty() )
						Server::GetInstance()->SendPacketReady( p->GetId(), msg.id );

				//모두 준비됐으면 게임 시작
				if ( IsGameStartAble() )
				{
					EVENTINFO ei { -1, m_roomNum,EEventType::TICK };
					EVENT ev { eventKey, ei, std::chrono::high_resolution_clock::now() + std::chrono::seconds( 1 )  , EEventType::TICK };
					Server::GetInstance()->m_timerQueue.push( ev );
					PTC_Player players[maxPlayer];
					for ( int i = 0; i < maxPlayer; ++i )
					{
						players[i].id = m_players[i]->GetId();
						players[i].x = m_players[i]->GetPosX();
						players[i].y = m_players[i]->GetPosY();
					}
					for ( auto&& p : m_players )
						Server::GetInstance()->SendPacketGameStart( p->GetId(), players );
					GameStart();
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
					if ( !p->IsEmpty() )
						Server::GetInstance()->SendPacketUnReady( p->GetId(), msg.id );
			}
			break;
			case CS_POSITION:
			{
#ifdef LOG_ON
				//std::cout << "recv " << msg.id << "'s pos : " << tmpPacket->x << ", " << tmpPacket->y << std::endl;
#endif
				csPacketPosition* tmpPacket = reinterpret_cast< csPacketPosition* >( packet );
				m_players[m_id2index[msg.id]]->SetPos( tmpPacket->x, tmpPacket->y );
				m_players[m_id2index[msg.id]]->SetSpeed( tmpPacket->speed );
			}
			break;
			case CS_ROTATE:
			{
				csPacketRotation* tmpPacket = reinterpret_cast< csPacketRotation* >( packet );
				m_players[m_id2index[msg.id]]->SetRot( tmpPacket->z );
			}
			break;
			case CS_ANIMATION:
			{
				csPacketAnimation* tmpPacket = reinterpret_cast< csPacketAnimation* >( packet );
				m_players[m_id2index[msg.id]]->SetAnim( tmpPacket->anim );
				for ( auto&& p : m_players )
					if ( !p->IsEmpty() && p->GetId() != msg.id )
						Server::GetInstance()->SendPacketAnimation( p->GetId(), msg.id, m_players[m_id2index[msg.id]]->GetAnimation() );
			}
			break;
			case CS_ATTACK:
			{
#ifdef LOG_ON
				std::cout << "[" << msg.id << "] recv attack packet" << std::endl;
#endif
				EVENTINFO ei { msg.id, m_roomNum, EEventType::SKILLDELAY };
				EVENT ev { eventKey, ei, std::chrono::high_resolution_clock::now() + std::chrono::seconds( attackDelay[m_players[m_id2index[msg.id]]->GetCharacterType()] )  , EEventType::SKILLDELAY };
				Server::GetInstance()->m_timerQueue.push( ev );
			}
			break;
			case CS_DEDUCT_HEART:
			{
#ifdef LOG_ON
				std::cout << "[" << msg.id << "] recv deduct heart packet" << std::endl;
#endif
				m_players[m_id2index[msg.id]]->DeductHeart();
				m_players[m_id2index[msg.id]]->SetInvincible( true );
				EVENTINFO ei { msg.id, m_roomNum, EEventType::INVINCIBLEDONE };
				EVENT ev { eventKey, ei, std::chrono::high_resolution_clock::now() + std::chrono::seconds( invincibleTime )  , EEventType::INVINCIBLEDONE };
				Server::GetInstance()->m_timerQueue.push( ev );
				for ( auto&& p : m_players )
					if ( !p->IsEmpty() )
						Server::GetInstance()->SendPacketDeductHeart( p->GetId(), msg.id, m_players[m_id2index[msg.id]]->GetHeartNum() );
				//죽었나?
				if ( m_players[m_id2index[msg.id]]->GetHeartNum() <= 0 )
				{
					m_players[m_id2index[msg.id]]->SetDie();
					for ( auto&& p : m_players )
						if ( !p->IsEmpty() )
							Server::GetInstance()->SendPacketDie( p->GetId(), msg.id );
					//게임 끝났는지 확인
					if ( IsGameOverAble() )
						GameOver();
				}
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
			default:
			{
				std::cout << "[" << msg.id << "] Invalid Packet Type Error! : " << ( int )( packet[1] ) << "\n";
			}
			break;
			}
		}
	}
	//in game send (pos, rot, attack)
	if ( m_isGameStarted )
	{
		for ( auto&& p1 : m_players )
			for ( auto&& p2 : m_players )
				if ( !p1->IsEmpty() && !p2->IsEmpty() && p1 != p2 )
				{
					Server::GetInstance()->SendPacketPosition( p1->GetId(), p2->GetId(), p2->GetPosX(), p2->GetPosY(), p2->GetSpeed() );
					Server::GetInstance()->SendPacketRotation( p1->GetId(), p2->GetId(), p2->GetRot() );
				}
	}
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

UxBool Room::IsGameOverAble()
{
	int count { 0 };
	for ( auto&& p : m_players )
		if ( !p->IsEmpty() && p->IsAlive() )
			++count;
	return count <= 1;
}

UxBool Room::IsHit( UxInt32 p1index, UxInt32 p2Index )
{
	if ( abs( m_players[p1index]->GetPosY() - m_players[p2Index]->GetPosY() ) > attackYRange[m_players[p1index]->GetCharacterType()] )
		return false;
	//left
	if ( m_players[p1index]->GetRot() > 0 )
	{
		UxSingle dis = m_players[p2Index]->GetPosX() - m_players[p1index]->GetPosX();
		if ( dis > 0 && dis <= attackXRange[m_players[p1index]->GetCharacterType()] )
			return true;
	}
	//right
	else
	{
		UxSingle dis = m_players[p1index]->GetPosX() - m_players[p2Index]->GetPosX();
		if ( dis > 0 && dis <= attackXRange[m_players[p1index]->GetCharacterType()] )
			return true;
	}
	return false;
}

UxBool Room::IsGameStartAble()
{
	for ( auto&& p : m_players )
		if ( p->IsEmpty() )return false;
	for ( auto&& p : m_players )
		if ( !p->GetReady() )return false;
	return true;
}

UxBool Room::EnterRoom( UxInt32 id, std::wstring name )
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
	PTC_Winner winner[4] { { -1,0 }, { -1,0 }, { -1,0 }, { -1,0 } };
	for ( int i = 0; i < maxPlayer; ++i )
	{
		winner[i].id = m_players[i]->GetId();
		if ( !m_players[i]->IsEmpty() && m_players[i]->IsAlive() )
			winner[i].isWin = 1;
		else
			winner[i].isWin = 0;
	}
	for ( auto&& p : m_players )
		if ( !p->IsEmpty() )
			Server::GetInstance()->SendPacketGameOver( p->GetId(), winner );
}

std::wstring Room::GetRoomName()
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

UxBool Room::GetDestroy()
{
	return m_destroy;
}
