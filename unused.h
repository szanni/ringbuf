#ifndef UNUSED

#ifdef __GNUC__
#  define UNUSED(x) UNUSED_ ## x __attribute__((__unused__))
#else
#  define UNUSED(x) UNUSED_ ## x
#endif

#endif

#ifndef UNUSED_FUNC

#ifdef __GNUC__
#  define UNUSED_FUNC __attribute__((__unused__))
#else
#  define UNUSED_FUNC
#endif

#endif

