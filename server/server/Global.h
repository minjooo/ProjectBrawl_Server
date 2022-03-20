

#pragma once
#include "Header.h"
#include "TypeDef.h"
#include "Socket.h"

constexpr size_t maxWorkerThread = 2;

constexpr UxInt32 noMsg = -1;
constexpr UxInt32 eventKey	= 10000;

constexpr UxInt32 maxBuffer = 4096;

//¹æ ÀÎ¿ø
constexpr UxInt32 maxPlayer = 2;
constexpr UxInt32 maxRoom	= 30;
constexpr UxInt32 maxClient = 120;
constexpr UxInt32 maxHeart	= 5;
constexpr UxInt32 notInRoom = -1;
constexpr UxUInt8 playTime	= 180 + 1;
constexpr UxInt32 invincibleTime = 2;

struct POSITION
{
	UxSingle x;
	UxSingle y;
};

constexpr POSITION startPosition[4] { {160.0, 670.0}, {590.0, 670.0}, {-790.0, 670.0}, {-190.0, 670.0} };

enum class EEventType
{
	RECV, SEND,
	TICK, GAMESTART,
	INVINCIBLEDONE
};

struct EVENTINFO
{
	UxInt32 id;
	UxInt32 roomNum;
	EEventType event_type;
};

struct EVENT {
	UxInt32 id;
	EVENTINFO info;
	std::chrono::high_resolution_clock::time_point wakeup_time;
	EEventType event_type;
	constexpr UxBool operator<( const EVENT& rhs ) const 
	{
		return wakeup_time > rhs.wakeup_time;
	}
};

struct OVER_EX 
{
	WSAOVERLAPPED		over;
	WSABUF				wsabuf[1];
	UxInt8				net_buf[maxBuffer];
	EEventType			ev_type;
};

struct SOCKETINFO {
	OVER_EX					recv_over {};
	std::shared_ptr<Socket>	socket {};

	UxUInt8 packet_buf[maxBuffer] {};
	UxInt32 prev_packet_data {};
	UxInt32 curr_packet_size {};

	UxBool	isConnected { false };
	std::string name { "" };
	std::atomic<UxInt32> roomNum { notInRoom };
};

struct message {
	UxInt32 id;
	std::string name;
	UxInt32 roomNum;
	UxChar buff[128];
};
