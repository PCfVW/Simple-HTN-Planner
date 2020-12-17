// ================================================================================================ File name: "EnumBlock.h"
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
    DECL_ENUM_ELEMENT(a),   // 1
    DECL_ENUM_ELEMENT(b),   // 2
    DECL_ENUM_ELEMENT(c),   // 3
    DECL_ENUM_ELEMENT(d),   // 4
    DECL_ENUM_ELEMENT(e),   // 5
    DECL_ENUM_ELEMENT(f),   // 6
    DECL_ENUM_ELEMENT(g),   // 7
    DECL_ENUM_ELEMENT(h),   // 8
    DECL_ENUM_ELEMENT(i),   // 9
    DECL_ENUM_ELEMENT(j),   // 10
    DECL_ENUM_ELEMENT(k),   // 11
    DECL_ENUM_ELEMENT(l),   // 12
    DECL_ENUM_ELEMENT(m),   // 13
    DECL_ENUM_ELEMENT(n),   // 14
    DECL_ENUM_ELEMENT(o),   // 15
    DECL_ENUM_ELEMENT(p),   // 16
    DECL_ENUM_ELEMENT(q),   // 17
    DECL_ENUM_ELEMENT(r),   // 18
    DECL_ENUM_ELEMENT(s),   // 19
    DECL_ENUM_ELEMENT(t)    // 20
}
END_ENUM(Block)

#endif // (!defined(ENUM_BLOCK_H) || defined(GENERATE_ENUM_STRINGS))