#pragma once

// compatibility with non-clang compilers...
#ifndef __has_attribute
#define __has_attribute(x) 0
#endif

#ifndef __has_feature
#define __has_feature(x) 0
#endif

#ifndef __has_builtin
#define __has_builtin(x) 0
#endif

#if __has_builtin(__builtin_expect)
#   ifdef __cplusplus
#      define LIKELY( exp )    (__builtin_expect( !!(exp), true ))
#      define UNLIKELY( exp )  (__builtin_expect( !!(exp), false ))
#   else
#      define LIKELY( exp )    (__builtin_expect( !!(exp), 1 ))
#      define UNLIKELY( exp )  (__builtin_expect( !!(exp), 0 ))
#   endif
#else
#   define LIKELY( exp )    (!!(exp))
#   define UNLIKELY( exp )  (!!(exp))
#endif

#if __has_attribute(maybe_unused)
#define UNUSED [[maybe_unused]]
#define UNUSED_IN_RELEASE [[maybe_unused]]
#elif __has_attribute(unused)
#define UNUSED __attribute__((unused))
#define UNUSED_IN_RELEASE __attribute__((unused))
#else
#define UNUSED
#define UNUSED_IN_RELEASE
#endif

#if defined(_MSC_VER) && _MSC_VER >= 1900
#    define RESTRICT __restrict
#elif (defined(__clang__) || defined(__GNUC__))
#    define RESTRICT __restrict__
#else
#    define RESTRICT
#endif