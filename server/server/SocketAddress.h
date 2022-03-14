

#pragma once


class SocketAddress
{
public:
	SocketAddress( UxUInt32 inAddress, UxUInt16 inPort );
	SocketAddress( const sockaddr& inSockAddr );
	SocketAddress();

public:
	size_t GetHash() const;
	UxInt32 GetSize() const;

public:
	UxBool operator==( const SocketAddress& inOther ) const;

private:
	UxUInt32& GetIP4Ref();
	const UxUInt32& GetIP4Ref()	const;

	sockaddr_in* GetAsSockAddrIn();
	const	sockaddr_in* GetAsSockAddrIn() const;

private:
	friend class Socket;
	sockaddr m_sockAddr;
};
