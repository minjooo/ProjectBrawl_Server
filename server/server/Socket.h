

#pragma once


class SocketAddress;
class Socket
{
public:
	Socket( SOCKET sock ) : m_socket( sock ) {};
	~Socket();

public:
	SOCKET GetSocket() { return m_socket; }

public:
	UxInt32 Connect(const SocketAddress& addr);
	UxInt32 Bind( const SocketAddress& addr );
	UxInt32 Listen( UxInt32 backlog = 32 );
	Socket* Accept( SocketAddress& fromAddr );

	int WSASend( LPWSABUF lpBuf, DWORD dwBufCnt, LPDWORD lpNumBytesSent, DWORD dwFlags, LPWSAOVERLAPPED lpOverllaped );
	int WSAReceive( LPWSABUF lpBuf, DWORD dwBufCnt, LPDWORD lpNumBytesRecvd, LPDWORD dwFlags, LPWSAOVERLAPPED lpOverllaped );

private:
	friend class SocketUtil;
	SOCKET	m_socket;
};

