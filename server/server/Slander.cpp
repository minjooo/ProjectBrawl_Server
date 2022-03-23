

#include "Header.h"
#include "TypeDef.h"
#include "Slander.h"


UxBool Slander::IsSlander( const std::wstring& name )
{
	std::ifstream f;
	f.open( "Slander.csv" );

	if ( !f.is_open() )
	{
		std::cout << "[Error] file open fail\n";
		return true;
	}

	//std::wstring str;
	//while ( f >> str )
	//	if ( name.find( str ) != std::wstring::npos )
	//		return true;

	
	return false;
}
