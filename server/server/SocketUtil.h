

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
	static Socket* CreateSocket( ESocketAddressFamily family );
	static UxVoid CleanUp();
	static UxVoid ReportError( const UxInt8* desc );
};

