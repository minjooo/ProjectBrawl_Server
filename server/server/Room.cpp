

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

	//update 호출 시작 필요?
}

UxVoid Room::Update()
{
	if ( this == nullptr )
		return;
	//update 필요
	//msgQueue내용 반영
	while ( !m_roomMsg.empty() )
	{
		char* msg = m_roomMsg.front();
		m_roomMsg.pop();

		switch ( msg[1] )
		{
		case CS_SELECT_CHARACTER:
		{
			//다 각각 반영하기
		}
		break;
		case CS_READY:
		{

		}
		break;
		case CS_UN_READY:
		{

		}
		break;
		case CS_POSITION:
		{

		}
		break;
		case CS_ROTATE:
		{

		}
		break;
		case CS_ANIMATION:
		{

		}
		break;
		case CS_ATTACK:
		{

		}
		break;
		case CS_HEART_DECREAS:
		{

		}
		break;
		case CS_DIE:
		{

		}
		break;
		default:
			break;
		}
	}

	//send
}

UxVoid Room::PushMsg( UxInt8* msg)
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

UxBool Room::EnterRoom( UxInt32 id, std::string name )
{
	if ( m_curPlayerNum >= maxPlayer )
		return false;

	//플레이어 추가 및 방 추가 처리 필요
	for ( auto&& player : m_players )
	{
		if ( player->IsEmpty() )
		{
			player->EnterRoom( id, name );
			++m_curPlayerNum;

			Server::GetInstance()->m_clients[id]->roomNum = m_roomNum;
			//들어왔다고 알라기 필요
			return true;
		}
	}

	return false;
}

UxVoid Room::LeaveRoom( UxInt32 id )
{
	//방 나감 처리 필요
	for ( auto&& player : m_players )
	{
		if ( player->GetId() == id )
		{
			player->Reset();
			--m_curPlayerNum;

			Server::GetInstance()->m_clients[id]->roomNum = notInRoom;
			//나갔다고 알라기 필요
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
