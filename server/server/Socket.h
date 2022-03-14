

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

	// Winsock2.h :: WSASend �����Լ� ù ��° ���� SOCKET�� ��� ���, ������ ���� COMPLETION_ROUTINE ������.
	int WSASend( LPWSABUF lpBuf, DWORD dwBufCnt, LPDWORD lpNumBytesSent, DWORD dwFlags, LPWSAOVERLAPPED lpOverllaped );

	// Winsock2.h :: WSARecv �����Լ� ù ��° ���� SOCKET�� ��� ���, ������ ���� COMPLETION_ROUTINE ������.
	int WSAReceive( LPWSABUF lpBuf, DWORD dwBufCnt, LPDWORD lpNumBytesRecvd, LPDWORD dwFlags, LPWSAOVERLAPPED lpOverllaped );

private:
	SOCKET	m_socket;
};

