#if defined(__STDC_NO_VLA__)
#    ifdef _MSC_VER
#        include <malloc.h>
#        define alloca _alloca
#    endif
#    define VLA_DECL(name, type, size) type* name = (type* )alloca(size)
#else
#    define VLA_DECL(name, type, size) type name[size]
#endif