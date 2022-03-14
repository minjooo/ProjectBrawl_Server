

#pragma once
#include "Header.h"
#include "TypeDef.h"
#include "Socket.h"

constexpr size_t maxWorkerThread = 2;

constexpr UxInt32 noMsg = -1;

constexpr UxInt32 maxBuffer = 256;
constexpr UxInt32 maxPlayer = 4;
constexpr UxInt32 maxRoom	= 50;
constexpr UxInt32 maxClient = 5100;

constexpr UxInt32 eventKey	= 10000;

enum class EEventType
{
	RECV, SEND,
	TICK
};

struct EVENT {
	UxInt32 id;
	UxInt32 target;
	std::chrono::high_resolution_clock::time_point wakeup_time;
	EEventType event_type;
	constexpr UxBool operator<( const EVENT& rhs ) const {
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
};

struct message {
	UxInt32 id;
	UxInt8	type;
};
