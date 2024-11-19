#ifndef DBG_H
#define DBG_H

#include <cstdio>  // for printf, vprintf
#include <cstdarg> // for va_list, va_start, va_end

// Check for standard debug-related macros if DEBUG is not explicitly defined
#ifndef DEBUG
    #if defined(_DEBUG) || !defined(NDEBUG)
        #define DEBUG 1
    #else
        #define DEBUG 0
    #endif
#endif

// Conditional debug logging based on the DEBUG flag
#if DEBUG
// Simple printf wrapper with immediate flushing for debug builds
inline void dbg(const char* format, ...) {
    va_list args;
    va_start(args, format);
    vprintf(format, args);
    va_end(args);
    printf("\n");
    fflush(stdout);
}
#else
// In production builds, dbg does nothing
inline void dbg(const char* format, ...) {}

#endif // DEBUG

#endif // DBG_H
