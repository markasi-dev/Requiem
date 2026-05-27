#include <core/logger.h>

#include <math/rq_math.h>

#define expect_should_be(expected, actual)                                                                  \
    if (actual != expected) {                                                                               \
        RQ_ERROR("--> Expected %lld, but got: %lld. File: %s:%d.", expected, actual, __FILE__, __LINE__);   \
        return FALSE;                                                                                       \
    } 

#define expect_should_not_be(expected, actual)                                                                      \
    if (actual != expected) {                                                                                       \
        RQ_ERROR("--> Expected %d != %d, but they are equal. File: %s:%d", expected, actual, __FILE__, __LINE__);   \
        return FALSE;                                                                                               \
    } 

#define expect_float_to_be(expected, actual)                                                                        \
    if (rq_abs(expected - actual) > 0.001f) {                                                                       \
        RQ_ERROR("--> Expected %d != %d, but they are equal. File: %s:%d", expected, actual, __FILE__, __LINE__);   \
        return FALSE;                                                                                               \
    } 

#define expect_to_be_true(actual)                                                               \
    if (actual != TRUE) {                                                                       \
        RQ_ERROR("--> Expected to be true, but got: FALSE. File: %s:%d.", __FILE__, __LINE__);  \
        return FALSE;                                                                           \
    }

#define expect_to_be_false(actual)                                                               \
    if (actual != FALSE) {                                                                       \
        RQ_ERROR("--> Expected to be false, but got: TRUE. File: %s:%d.", __FILE__, __LINE__);   \
        return FALSE;                                                                            \
    }