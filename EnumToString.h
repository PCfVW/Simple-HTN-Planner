// ================================================================================================ File name: "EnumToString.h"
// Adapted from https://www.codeproject.com/Articles/10500/Converting-C-enums-to-strings
//    - Added class line 12
//    - Added const line 14, 17 and 18.
//    - Removed enum line 14
#undef DECL_ENUM_ELEMENT
#undef BEGIN_ENUM
#undef END_ENUM

#ifndef GENERATE_ENUM_STRINGS
#define DECL_ENUM_ELEMENT( element ) element
#define BEGIN_ENUM( ENUM_NAME ) typedef enum class tag##ENUM_NAME
#define END_ENUM( ENUM_NAME ) ENUM_NAME; \
            const char* GetString##ENUM_NAME(##ENUM_NAME index);
#else
#define DECL_ENUM_ELEMENT( element ) #element
#define BEGIN_ENUM( ENUM_NAME ) const char* gs_##ENUM_NAME [] =
#define END_ENUM( ENUM_NAME ) ; const char* GetString##ENUM_NAME(enum \
            tag##ENUM_NAME index){ return gs_##ENUM_NAME [index]; }
#endif