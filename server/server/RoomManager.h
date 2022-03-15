

#pragma once
#include "Room.h"

class RoomManager
{
public:
	RoomManager();
	~RoomManager();

public:
	UxVoid UpdateRooms();
	UxInt32 GetNextRoomCounter();
	UxInt32 AddNewRoom( std::string name );
	UxVoid DestroyRoom( UxInt32 roomNum );

public:
	std::map<UxInt32, Room*> m_rooms;
	
private:
	UxInt32 m_roomCounter;
};
