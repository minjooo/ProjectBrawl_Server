

#pragma once
#include "Player.h"


class Room
{
public:
	Room( UxInt32 room_num );
	Room( UxInt32 room_num, std::wstring room_name );
	~Room();

public:
	UxVoid Initialize( UxInt32 room_num );
	UxVoid Update();

	UxBool IsEmpty();
	UxBool IsGameStarted();

	UxBool EnterRoom( UxInt32 id, std::wstring name );
	UxVoid LeaveRoom( UxInt32 id );

	std::wstring GetRoomName();
	UxInt32 GetcurPlayerNum();
	UxInt32 GetRoomNum();

	UxVoid GameStart();
	UxVoid GameOver( UxInt32 winner );

private:
	UxBool m_isGameStarted;

	UxInt32 m_roomNum;
	std::wstring m_roomName;

	UxInt32 m_curPlayerNum;
	std::array<Player*, maxPlayer> m_players;
};

