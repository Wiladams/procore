
#include <cstdint>
#include <cstddef>		// nullptr_t, ptrdiff_t, size_t
#include <cstring>

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