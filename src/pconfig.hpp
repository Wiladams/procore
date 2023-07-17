#ifndef PCORE_PCONFIG_HPP
#define PCORE_PCONFIG_HPP

#pragma once



// Herein lie definitions of some things that are low level enough 
// to be universal
// Don't put Windows specifics in here. Put that stuff in the w32_***
// files.
//
// Put things like universal typedefs and the like in here
//
// If this file hasn't already been included, you should include
// it as the first item to be included.
//

#include <cstdint>		// uint8_t, etc
#include <cstddef>		// nullptr_t, ptrdiff_t, size_t

// Windows DLL support
#ifdef _WIN32
#define INLINE __forceinline
#ifdef PROCORE_STATIC_DEFINE
    #define PROCORE_DECL
    #define PROCORE_EXP
#else
    #define PROCORE_EXP __declspec(dllexport)
    #ifndef PROCORE_DECL
        #ifdef PROCORE_EXPORTS
            // We are building this library, so the classes must be exported
            #define PROCORE_DECL __declspec(dllexport)
        #else
            // We are using this library, so the classes must be imported
            #define PROCORE_DECL __declspec(dllimport)
        #endif
    #endif
#endif
# pragma warning(disable: 4251)
# pragma warning(disable: 4275)
# if (_MSC_VER >= 1400) // vc8 or higher
#  pragma warning(disable: 4996) // disable all deprecation warnings
# endif
#else
#define INLINE static inline
#  if __GNUC__ >= 4
#  define PROCORE_EXP __attribute__ ((visibility ("default")))
#  define PROCORE_DECL __attribute__ ((visibility ("default")))
#  define PROCORE_IMP __attribute__ ((visibility ("default")))
#  else
#  define PROCORE_EXP
#  define PROCORE_DECL
#  define PROCORE_IMP
#  endif
#endif



namespace pcore 
{
    // Determine at runtime if the CPU is little-endian (intel standard)
    static INLINE bool isLE() noexcept {int i = 1; return (int)*((unsigned char*)&i) == 1;}
    static INLINE bool isBE() noexcept { return !isLE(); }
}

// Some very common types
// Usually the 'uintX_t' should be used
namespace {
    //using byte = uint8_t;

    using uchar = unsigned char;
    using uint = unsigned int;
    using ushort = uint16_t;

    using f32 = float;
    using f64 = double;
}

#endif // PCORE_PCONFIG_HPP