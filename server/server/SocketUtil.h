

#pragma once
#include "Socket.h"

enum ESocketAddressFamily {
	INET = AF_INET,
	INET6 = AF_INET6
};

class SocketUtil
{
public:
	static UxBool StaticInitialize();
	static std::shared_ptr<Socket> CreateSocket( ESocketAddressFamily family );
	static UxVoid CleanUp();
};

