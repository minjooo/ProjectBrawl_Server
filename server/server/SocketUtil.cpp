

#include "Header.h"
#include "TypeDef.h"
#include "Socket.h"
#include "SocketUtil.h"


UxBool SocketUtil::StaticInitialize()
{
	WSADATA wsaData;
	int result = WSAStartup( MAKEWORD( 2, 2 ), &wsaData );
	if ( result != NO_ERROR ) 
	{
		ReportError( "StartUp" );
		return false;
	}
	return true;
}

UxVoid SocketUtil::CleanUp()
{
	WSACleanup();
}

UxVoid SocketUtil::ReportError( const UxInt8* desc )
{
	WCHAR* buf;
	DWORD errorNum = WSAGetLastError();

	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM |
		FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		errorNum,
		MAKELANGID( LANG_NEUTRAL, SUBLANG_DEFAULT ),
		( LPTSTR )&buf, 0, NULL );

	std::wcout << L"ERROR " << errorNum << " : " << desc << ", " << buf << std::endl;
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
		ReportError( "CreateSocket" );
		return nullptr;
	}
}
