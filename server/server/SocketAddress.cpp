

#include "Header.h"
#include "TypeDef.h"
#include "SocketAddress.h"


SocketAddress::SocketAddress( UxUInt32 inAddress, UxUInt16 inPort )
{
	GetAsSockAddrIn()->sin_family = AF_INET;
	GetIP4Ref() = htonl( inAddress );
	GetAsSockAddrIn()->sin_port = htons( inPort );
}

SocketAddress::SocketAddress( const sockaddr& inSockAddr )
{
	memcpy( &m_sockAddr, &inSockAddr, sizeof( sockaddr ) );
}

SocketAddress::SocketAddress()
{
	GetAsSockAddrIn()->sin_family = AF_INET;
	GetIP4Ref() = INADDR_ANY;
	GetAsSockAddrIn()->sin_port = 0;
}

UxBool SocketAddress::operator==( const SocketAddress& inOther ) const
{
	return ( m_sockAddr.sa_family == AF_INET &&
		GetAsSockAddrIn()->sin_port == inOther.GetAsSockAddrIn()->sin_port ) &&
		( GetIP4Ref() == inOther.GetIP4Ref() );
}

size_t SocketAddress::GetHash() const
{
	return ( GetIP4Ref() ) |
		( ( static_cast< UxUInt32 >( GetAsSockAddrIn()->sin_port ) ) << 13 ) |
		m_sockAddr.sa_family;
}

UxInt32 SocketAddress::GetSize() const 
{
	return sizeof( sockaddr ); 
}

UxUInt32& SocketAddress::GetIP4Ref() 
{ 
	return *reinterpret_cast< UxUInt32* >( &GetAsSockAddrIn()->sin_addr.S_un.S_addr ); 
}

const UxUInt32& SocketAddress::GetIP4Ref()	const 
{
	return *reinterpret_cast< const UxUInt32* >( &GetAsSockAddrIn()->sin_addr.S_un.S_addr ); 
}

sockaddr_in* SocketAddress::GetAsSockAddrIn() 
{ 
	return reinterpret_cast< sockaddr_in* >( &m_sockAddr );
}

const	sockaddr_in* SocketAddress::GetAsSockAddrIn() const 
{ 
	return reinterpret_cast< const sockaddr_in* >( &m_sockAddr );
}
