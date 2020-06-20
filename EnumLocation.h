// File name: "EnumLocation.h"
#if ( !defined(ENUM_LOCATION_H) || defined(GENERATE_ENUM_STRINGS) )

#if (!defined(GENERATE_ENUM_STRINGS))
#define ENUM_LOCATION_H
#endif

#include "EnumToString.h"

///////////////////////////////
// The enum declaration
///////////////////////////////
BEGIN_ENUM(Location)
{
	DECL_ENUM_ELEMENT(none),
	DECL_ENUM_ELEMENT(home),
	DECL_ENUM_ELEMENT(park)
}
END_ENUM(Location)

#endif // (!defined(ENUM_LOCATION_H) || defined(GENERATE_ENUM_STRINGS))