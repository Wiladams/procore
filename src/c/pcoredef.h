
#include <stdint.h>   // uint16_t, uint32_t, uint64_t
#include <string.h>   // memcpy, memset, memcmp
#include <stddef.h>   // NULL, size_t, ptrdiff_t
#include <stdbool.h>  // bool, true, false
#include <assert.h>   // assert


#if defined(__cplusplus) && __cplusplus >= 201703L
  // Function typedefs are `noexcept`, however, it's not available until C++17.
  #define PC_NOEXCEPT noexcept
#else
  #define PC_NOEXCEPT
#endif

#if defined(__cplusplus)
  #define PC_NOEXCEPT_C noexcept
#else
  #define PC_NOEXCEPT_C
#endif

// Define the `INLINE` macro for the compiler.
#if __STDC_VERSION__ >= 199901L
  #define INLINE inline
#elif defined(_MSC_VER)
  #define INLINE __forceinline
#else
  #define INLINE __attribute__((always_inline)) inline
#endif

#ifdef _MSC_VER
  #ifndef unlikely
    #define unlikely(x) x
  #endif // unlikely
#else
  #ifndef unlikely
    #define unlikely(x) __builtin_expect(!!(x), 0)
  #endif // unlikely
#endif
