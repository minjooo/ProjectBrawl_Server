

#include "Header.h"
#include "TypeDef.h"
#include "RoomManager.h"


RoomManager::RoomManager()
{
	m_roomCounter = 0;
}

RoomManager::~RoomManager()
{
	for ( auto& room : m_rooms )
	{
		delete room.second;
	}
	m_rooms.clear();
}

UxVoid RoomManager::UpdateRooms()
{
	//인원 없는 방 제거
	std::vector<UxInt32> desVec;
	for ( auto&& room : m_rooms )
	{
		if ( room.second->GetDestroy() )
		{
			desVec.push_back( room.first );
		}
	}
	if ( desVec.size() > 0 )
		for ( auto&& i : desVec )
			DestroyRoom( i );

	for ( auto&& room : m_rooms )
	{
		room.second->Update();
	}
}

UxInt32 RoomManager::GetNextRoomCounter()
{
	return ++m_roomCounter;
}

UxInt32 RoomManager::AddNewRoom( std::wstring name )
{
	++m_roomCounter;
	m_rooms.insert( std::make_pair( m_roomCounter, new Room( m_roomCounter, name ) ) );

	return m_roomCounter;
}

UxVoid RoomManager::DestroyRoom( UxInt32 roomNum )
{
	m_rooms[roomNum]->~Room();
	m_rooms.erase( roomNum );
}
