

#pragma once
#include "TypeDef.h"
#include "Global.h"
#include "Singleton.h"
#include "SocketUtil.h"
#include "RoomManager.h"


class ThreadHandler;
class Socket;
class Server : public Singleton<Server>
{
public:
	Server();
	~Server();

public:
	UxVoid	Initialize();
	UxVoid	Run();

public:
	UxVoid CreateClientsSlot();
	SOCKADDR_IN GetServerAddr();

public:
	//send들


public:
	HANDLE	m_iocp;
	std::array<SOCKETINFO*, maxClient> m_clients;

	//락프리 필요..
	std::priority_queue<EVENT> m_timerQueue;
	std::queue<message> m_roomMsgQueue;

	RoomManager				m_roomManager;

private:
	ThreadHandler*			m_ThreadHandler;

	SocketUtil				m_sockUtil;

	std::shared_ptr<Socket> m_listenSocket;
	SOCKADDR_IN				m_ServerAddr {};
};

