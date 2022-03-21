

#include "Header.h"
#include "TypeDef.h"
#include "Slander.h"


UxBool Slander::IsSlander( const UxString& name )
{
	std::ifstream f;
	f.open( "Slander.csv" );

	if ( !f.is_open() )
	{
		std::cout << "[Error] file open fail\n";
		return true;
	}

	UxString str;
	while ( f >> str )
		if ( name.find( str ) != UxString::npos )
			return true;

	return false;
}
