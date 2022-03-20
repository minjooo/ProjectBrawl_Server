

#include "Header.h"
#include "TypeDef.h"
#include "Socket.h"
#include "SocketUtil.h"


bool SocketUtil::StaticInitialize()
{
	WSADATA wsaData;
	int result = WSAStartup( MAKEWORD( 2, 2 ), &wsaData );
	if ( result != NO_ERROR ) {
		return false;
	}
	return true;
}

void SocketUtil::CleanUp()
{
	WSACleanup();
}

Socket* SocketUtil::CreateSocket( ESocketAddressFamily family )
{
	SOCKET s = WSASocket( family, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED );

	if ( s != INVALID_SOCKET )
	{
		return  new Socket( s );
	}
	else
	{
		return nullptr;
	}
}
