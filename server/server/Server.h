

#pragma once
#include "TypeDef.h"
#include "Global.h"
#include "Singleton.h"
#include "SocketUtil.h"


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

private:
	ThreadHandler*			m_ThreadHandler;

	std::shared_ptr<Socket> m_listenSocket;
	SocketUtil				m_sockUtil;
	SOCKADDR_IN				m_ServerAddr {};
};

