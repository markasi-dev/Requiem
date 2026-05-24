#pragma once

#include "defines.h"

typedef union vec2_u {
    f32 elements[2];
    struct {
        union {
            // First element.
            f32 x, r, s, u;
        };
        union {
            // Second element.
            f32 y, g, t, v;
        };
    };
} vec2;

typedef union vec3_u {
    union {
        f32 elements[3];
        struct {
            union {
                // First element.
                f32 x, r, s, u;
            };
            union {
                // Second element.
                f32 y, g, t, v;
            };
            union {
                // Third element.
                f32 z, b, p, w;
            };
        };
    };
} vec3;

typedef union vec4_u {
#if defined(RQ_USE_SIMD)
    alignas(16) __m128 data;
#endif
    alignas(16) f32 elements[4];
    union {
        struct {
            union {
                // First element.
                f32 x, r, s;
            };
            union {
                // Second element.
                f32 y, g, t;
            };
            union {
                // Third element.
                f32 z, b, p;
            };
            union {
                f32 w, a, q;
            };
        };
    };
} vec4;

typedef vec4 quat;

typedef union mat4_u {
    alignas(16) f32 data[16];

#if defined(RQ_USE_SIMD) 
    alignas(16) vec4 rows[4];
#endif
} mat4; 