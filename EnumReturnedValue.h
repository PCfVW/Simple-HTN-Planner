// ================================================================================================ File name: "EnumReturnedValue.h"
#if ( !defined(ENUM_RETURNED_VALUE_H) || defined(GENERATE_ENUM_STRINGS) )

#if (!defined(GENERATE_ENUM_STRINGS))
#define ENUM_RETURNED_VALUE_H
#endif

#include "EnumToString.h"

///////////////////////////////
// The enum declaration
///////////////////////////////
BEGIN_ENUM(ReturnedValue)
{
	DECL_ENUM_ELEMENT(None),
	DECL_ENUM_ELEMENT(False),
	DECL_ENUM_ELEMENT(True)
}
END_ENUM(ReturnedValue)

#endif // (!defined(ENUM_RETURNED_VALUE_H) || defined(GENERATE_ENUM_STRINGS))