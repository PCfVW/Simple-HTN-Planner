// ================================================================================================ File name: "EnumBlockStatus.h"
#if ( !defined(ENUM_BLOCK_STATUS_H) || defined(GENERATE_ENUM_STRINGS) )

#if (!defined(GENERATE_ENUM_STRINGS))
#define ENUM_BLOCK_STATUS_H
#endif

#include "EnumToString.h"

///////////////////////////////
// The enum declaration
///////////////////////////////
BEGIN_ENUM(BlockStatus)
{
    DECL_ENUM_ELEMENT(none),
        DECL_ENUM_ELEMENT(done),
        DECL_ENUM_ELEMENT(inaccessible),
        DECL_ENUM_ELEMENT(move_to_table),
        DECL_ENUM_ELEMENT(move_to_block),
        DECL_ENUM_ELEMENT(waiting)
}
END_ENUM(BlockStatus)

#endif // (!defined(ENUM_BLOCK_STATUS_H) || defined(GENERATE_ENUM_STRINGS))