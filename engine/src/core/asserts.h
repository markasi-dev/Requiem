#pragma once

#include "defines.h"

// Disable assertions by commenting out the line below
#define RQ_ASSERTIONS_ENABLED

// Check if the assertions are enabled.
#ifdef RQ_ASSERTIONS_ENABLED
    #if _MSC_VER
        #include <intrin.h>
        #define debugBreak() __debugbreak()
    #else
        #define debugBreak() __builtin_trap()
    #endif


    RAPI void report_assertion_failure(const char* expression, const char* msg, const char* file, i32 line);

    // Custom assertion functions
    #define RQ_ASSERT(expr)                                              \
        {                                                                \
            if (expr) {                                                  \
            } else {                                                     \
                report_assertion_failure(#expr, "", __FILE__, __LINE__); \
            }                                                            \
        }                                                                

    #define RQ_ASSERT_MSG(expr, msg)                                     \
        {                                                                \
            if (expr) {                                                  \
            } else {                                                     \
                report_assertion_failure(#expr, msg, __FILE__, __LINE__) \
            }                                                            \
        }                                                                

    #ifdef _DEBUG
        #define RQ_ASSERT_DEBUG(expr)                                        \
            {                                                                \
            if (expr) {                                                     \
                } else {                                                     \
                    report_assertion_failure(#expr, "", __FILE__, __LINE__); \
                    debugBreak();                                            \
                }                                                            \
            } 
            
        #else        
            #define RQ_ASSERT_DEBUG(expr) // Does nothing at all.
    #endif 
    
#else
    // Just make them do nothing at all, Jerald.
    #define RQ_ASSERT(expr)
    #define RQ_ASSERT_MSG(expr, msg)
    #define RQ_ASSERT_DEBUG(expr)   
#endif
