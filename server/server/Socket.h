

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
	std::shared_ptr<Socket> Accept( SocketAddress& fromAddr );

	// Winsock2.h :: WSASend 래핑함수 첫 번째 인자 SOCKET은 멤버 사용, 마지막 인자 COMPLETION_ROUTINE 사용안함.
	int WSASend( LPWSABUF lpBuf, DWORD dwBufCnt, LPDWORD lpNumBytesSent, DWORD dwFlags, LPWSAOVERLAPPED lpOverllaped );

	// Winsock2.h :: WSARecv 래핑함수 첫 번째 인자 SOCKET은 멤버 사용, 마지막 인자 COMPLETION_ROUTINE 사용안함.
	int WSAReceive( LPWSABUF lpBuf, DWORD dwBufCnt, LPDWORD lpNumBytesRecvd, LPDWORD dwFlags, LPWSAOVERLAPPED lpOverllaped );

private:
	SOCKET	m_socket;
};

