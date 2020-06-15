#pragma once // File name: "EnumBlock.h"
#if ( !defined(ENUM_BLOCK_H) || defined(GENERATE_ENUM_STRINGS) )

#if (!defined(GENERATE_ENUM_STRINGS))
#define ENUM_BLOCK_H
#endif

#include "EnumToString.h"

///////////////////////////////
// The enum declaration
///////////////////////////////
BEGIN_ENUM(Block)
{
    DECL_ENUM_ELEMENT(none),
    DECL_ENUM_ELEMENT(hand),
    DECL_ENUM_ELEMENT(table),
    DECL_ENUM_ELEMENT(a),
    DECL_ENUM_ELEMENT(b),
    DECL_ENUM_ELEMENT(c),
    DECL_ENUM_ELEMENT(d)
}
END_ENUM(Block)

#endif // (!defined(ENUM_BLOCK_H) || defined(GENERATE_ENUM_STRINGS))
