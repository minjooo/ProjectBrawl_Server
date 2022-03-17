

#pragma once
#include "MyThread.h"
#include "Global.h"


class WorkerThread final : public MyThread
{
public:
	UxVoid InitThread() override;
	UxVoid ProcThread() override;
	UxVoid JoinThread() override;

	UxVoid ProcPacket( UxInt32 id, UxVoid* buff );

	UxVoid DisconnectClient( UxInt32 clientID );

private:
	SOCKADDR_IN m_ServerAddr {};
	UxInt32		m_addrLen {};
};

