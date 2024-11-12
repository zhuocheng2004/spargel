#ifndef BASE_COMPILER_HAS_BUILTIN_H_
#define BASE_COMPILER_HAS_BUILTIN_H_

#ifdef __has_builtin
#define HAS_BUILTIN(x) __has_builtin(x)
#else
#define HAS_BUILTIN(x) 0
#endif

#endif
