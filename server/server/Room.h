

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
	UxVoid PushMsg( const message& msg );
	UxBool IsEmpty();
	UxBool IsGameStartAble();
	UxBool IsGameStarted();

	UxBool EnterRoom( UxInt32 id, std::string name );
	UxVoid LeaveRoom( UxInt32 id );

	std::string GetRoomName();
	UxInt32 GetcurPlayerNum();
	UxInt32 GetRoomNum();

	UxVoid GameStart();
	UxVoid GameOver();

private:
	std::queue<message> m_roomMsg;
	UxBool m_isGameStarted;
	UxUInt8 m_leftTime;

	UxInt32 m_roomNum;
	std::string m_roomName;

	UxInt32 m_curPlayerNum;
	std::array<Player*, maxPlayer> m_players;
};

