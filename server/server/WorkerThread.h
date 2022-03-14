

#pragma once
#include "MyThread.h"
#include "Global.h"


class WorkerThread final : public MyThread
{
public:
	UxVoid InitThread() override;
	UxVoid ProcThread() override;
	UxVoid JoinThread() override;

	message ProcPacket( int id, void* buff );

	UxVoid DisconnectClient( int clientID, SOCKET client );

private:
	SOCKADDR_IN m_ServerAddr {};
	UxInt32		m_addrLen {};
};

