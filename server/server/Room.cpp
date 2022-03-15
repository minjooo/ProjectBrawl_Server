

#include "Header.h"
#include "Global.h"
#include "TypeDef.h"
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

UxBool Room::EnterRoom( UxInt32 id, std::wstring name )
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
			//방번호 기억 필요 어디에..?
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
			//방번호 기억 필요 어디에..?
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
