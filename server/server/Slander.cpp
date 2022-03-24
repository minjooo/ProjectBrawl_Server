

#include "Header.h"
#include "TypeDef.h"
#include "Slander.h"

std::string WS2S( std::wstring const& str, std::locale const& loc = std::locale("kor") )
{
	typedef std::codecvt<wchar_t, char, std::mbstate_t> codecvt_t;
	codecvt_t const& codecvt = std::use_facet<codecvt_t>( loc );
	std::mbstate_t state;

	std::vector<char> buf( ( str.size() + 1 ) * codecvt.max_length() );
	wchar_t const* in_next = str.c_str();
	char* out_next = &buf[0];
	codecvt_t::result r = codecvt.out( state, str.c_str(), str.c_str() + str.size(), in_next, &buf[0], &buf[0] + buf.size(), out_next );
	return std::string( &buf[0] );
}

UxBool Slander::IsSlander( const std::wstring& name )
{
	std::ifstream f;
	f.open( "Slander.csv" );

	if ( !f.is_open() )
	{
		std::cout << "[Error] file open fail\n";
		return true;
	}

	UxString str_name = WS2S( name );
	UxString str;
	while ( f >> str )
	{
		if ( str_name.find( str ) != std::wstring::npos )
			return true;
	}
	
	return false;
}
