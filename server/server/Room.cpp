

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

	//update ȣ�� ���� �ʿ�?
}

UxVoid Room::Update()
{
	if ( this == nullptr )
		return;
	//update �ʿ�
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

	//�÷��̾� �߰� �� �� �߰� ó�� �ʿ�
	for ( auto&& player : m_players )
	{
		if ( player->IsEmpty() )
		{
			player->EnterRoom( id, name );
			++m_curPlayerNum;
			//���ȣ ��� �ʿ� ���..?
			//���Դٰ� �˶�� �ʿ�
			return true;
		}
	}

	return false;
}

UxVoid Room::LeaveRoom( UxInt32 id )
{
	//�� ���� ó�� �ʿ�
	for ( auto&& player : m_players )
	{
		if ( player->GetId() == id )
		{
			player->Reset();
			--m_curPlayerNum;
			//���ȣ ��� �ʿ� ���..?
			//�����ٰ� �˶�� �ʿ�
			break;
		}
	}
}

UxVoid Room::GameStart()
{
	//��� �÷��̾�� ���� ���� ��Ŷ �����ֱ�

	m_isGameStarted = true;
}

UxVoid Room::GameOver( UxInt32 winner )
{
	m_isGameStarted = false;

	//��� �÷��̾�� ���� �˷��ֱ�
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
