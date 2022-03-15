

#include "Header.h"
#include "TypeDef.h"
#include "RoomManager.h"


RoomManager::RoomManager()
{

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
	for ( auto&& room : m_rooms )
	{
		room.second->Update();
	}
}

UxInt32 RoomManager::GetNextRoomCounter()
{
	return ++m_roomCounter;
}

UxInt32 RoomManager::AddNewRoom( std::string name )
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
