

#pragma once
#include "Player.h"
#include "Global.h"


class Room
{
public:
	Room( UxInt32 room_num );
	Room( UxInt32 room_num, std::string room_name );
	~Room();

public:
	UxVoid Initialize( UxInt32 room_num );
	UxVoid Update();

public:
	UxVoid PushMsg( UxInt8* msg);
	UxBool IsEmpty();
	UxBool IsGameStarted();

	UxBool EnterRoom( UxInt32 id, std::string name );
	UxVoid LeaveRoom( UxInt32 id );

	std::string GetRoomName();
	UxInt32 GetcurPlayerNum();
	UxInt32 GetRoomNum();

	UxVoid GameStart();
	UxVoid GameOver( UxInt32 winner );

private:
	std::queue<UxInt8*> m_roomMsg;
	UxBool m_isGameStarted;

	UxInt32 m_roomNum;
	std::string m_roomName;

	UxInt32 m_curPlayerNum;
	std::array<Player*, maxPlayer> m_players;
};

