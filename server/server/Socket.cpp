

#include "Header.h"
#include "TypeDef.h"
#include "SocketAddress.h"
#include "Socket.h"


Socket::~Socket()
{
	LINGER linger { 1, 0 };
	setsockopt( m_socket, SOL_SOCKET, SO_LINGER, ( UxInt8* )&linger, sizeof( linger ) );
	shutdown( m_socket, SD_SEND );
	closesocket( m_socket );
}

UxInt32 Socket::Connect( const SocketAddress& address )
{
	UxInt32 result = ::connect( m_socket, &address.m_sockAddr, address.GetSize() );
	if ( result == SOCKET_ERROR )
	{
		return WSAGetLastError();
	}
	return NO_ERROR;
}

UxInt32 Socket::Bind( const SocketAddress& address )
{
	UxInt32 result = ::bind( m_socket, &address.m_sockAddr, address.GetSize() );
	if ( result == SOCKET_ERROR ) 
	{
		return WSAGetLastError();
	}
	return NO_ERROR;
}

UxInt32 Socket::Listen( UxInt32 backLog )
{
	UxInt32 result = ::listen( m_socket, backLog );
	if ( result == SOCKET_ERROR )
	{
		std::cout << "Listen Error!!!!" << std::endl;
	}
	return NO_ERROR;
}

std::shared_ptr<Socket> Socket::Accept( SocketAddress& fromAddr )
{
	UxInt32 length { static_cast< UxInt32 >( fromAddr.GetSize() ) };
	SOCKET newSocket = ::WSAAccept( m_socket, &fromAddr.m_sockAddr, &length, NULL, NULL );

	if ( newSocket != INVALID_SOCKET ) 
	{
		std::cout << "Socket Created" << std::endl;
		return std::shared_ptr<Socket>( new Socket( newSocket ) );
	}
	else
		return nullptr;
}

UxInt32 Socket::WSASend( LPWSABUF lpBuf, DWORD dwBufCnt, LPDWORD lpNumBytesSent, DWORD dwFlags, LPWSAOVERLAPPED lpOverllaped )
{
	if ( this == nullptr ) 
	{
		std::cout << "nullptr references." << std::endl;
		return -1;
	}

	UxInt32 bytesSent { ::WSASend( m_socket, lpBuf, dwBufCnt, lpNumBytesSent, dwFlags, lpOverllaped, NULL ) };
	if ( bytesSent == SOCKET_ERROR ) 
	{
		UxInt32 errNum { WSAGetLastError() };
		if ( errNum != WSA_IO_PENDING ) 
		{
			return errNum;
		}
	}
	//´úº¸³ÂÀ»¶§ Ã³¸® ÇÊ¿ä
	return bytesSent;
}

UxInt32 Socket::WSAReceive( LPWSABUF lpBuf, DWORD dwBufCnt, LPDWORD lpNumBytesRecvd, LPDWORD dwFlags, LPWSAOVERLAPPED lpOverllaped )
{
	UxInt32 bytesRecvd { ::WSARecv( m_socket, lpBuf, dwBufCnt, lpNumBytesRecvd, dwFlags, lpOverllaped, NULL ) };
	if ( bytesRecvd == SOCKET_ERROR ) 
	{
		UxInt32 errNum { WSAGetLastError() };
		if ( errNum != WSA_IO_PENDING ) 
		{
			return errNum;
		}
	}
	return bytesRecvd;
}
