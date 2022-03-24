

#pragma once
#include "Header.h"
#include "Protocol.h"
#include "TypeDef.h"
#include "Socket.h"

constexpr size_t maxWorkerThread = 2;

constexpr UxInt32 noMsg = -1;
constexpr UxInt32 eventKey	= 10000;

constexpr UxInt32 maxBuffer = 4096;

//방 인원
constexpr UxInt32 maxPlayer = 2;
constexpr UxInt32 maxRoom	= 30;
constexpr UxInt32 maxClient = 120;
constexpr UxInt32 maxHeart	= 5;
constexpr UxInt32 notInRoom = -1;

constexpr UxUInt8 playTime	= 180 + 1;
constexpr UxInt32 coolTime	= 3;
constexpr UxInt32 invincibleTime = 2;

struct POSITION
{
	UxSingle x;
	UxSingle y;
};

constexpr POSITION startPosition[4] { {0, 670.0}, {-230, 670.0}, {-790.0, 670.0}, {-420.0, 670.0} };

//none 디트 엔지 소드 위치 로그 순
constexpr UxInt8 hitType[6] { 0, HIT_KNOCKDOWN, HIT_KNOCKBACK, HIT_KNOCKDOWN , HIT_KNOCKBACK, HIT_KNOCKBACK };
constexpr UxInt32 attackXRange[6] { 0, 250, 700, 300, 500, 700 };
constexpr UxInt32 attackYRange[6] { 0, 250, 150, 200, 200, 120 };
//constexpr UxInt32 attackYRange = 180;
constexpr UxInt32 attackDelay[6] { 0, 0.4, 0.4, 0.2, 0.6, 0.2 };

enum class EEventType
{
	RECV, SEND,
	TICK, GAMESTART,
	INVINCIBLEDONE, RESETCOOLTIME, SKILLDELAY
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
	OVER_EX	recv_over {};
	Socket*	socket;

	UxUInt8 packet_buf[maxBuffer] {};
	UxInt32 prev_packet_data {};
	UxInt32 curr_packet_size {};

	UxBool	isConnected { false };
	std::wstring name = L"";
	std::atomic<UxInt32> roomNum { notInRoom };
};

struct message {
	UxInt32 id;
	std::wstring name;
	UxInt32 roomNum;
	UxChar buff[128];
};
