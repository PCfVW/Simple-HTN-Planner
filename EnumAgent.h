// ================================================================================================ File name: "EnumAgent.h"
#if ( !defined(ENUM_AGENT_H) || defined(GENERATE_ENUM_STRINGS) )

#if (!defined(GENERATE_ENUM_STRINGS))
#define ENUM_AGENT_H
#endif

#include "EnumToString.h"

///////////////////////////////
// The enum declaration
///////////////////////////////
BEGIN_ENUM(Agent)
{
    DECL_ENUM_ELEMENT(none),
    DECL_ENUM_ELEMENT(me),
    DECL_ENUM_ELEMENT(taxi)
}
END_ENUM(Agent)

#endif // (!defined(ENUM_AGENT_H) || defined(GENERATE_ENUM_STRINGS))