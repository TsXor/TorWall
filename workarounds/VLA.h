#if defined(__GNUC__)
#    define VLA_DECL(name, type, size) type name[size]
#else
#    ifdef _MSC_VER
#        include <malloc.h>
#        define alloca _alloca
#    endif
#    define VLA_DECL(name, type, size) type* name = (type* )alloca(size)
#endif