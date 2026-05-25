#include "rq_math.h"
#include "platform/platform.h"

#include <math.h>
#include <stdlib.h>

static b8 rand_seeded = FALSE;

/**
 * NOTE: These exist because we dont want to be importing the 
 * entire <math.h> header. That is just SOOOOOO ugly ughhhh... (AND BIG!!)
 */
f32 rq_sin(f32 x) {
    return sinf(x);
}

f32 rq_cos(f32 x)  {
    return cosf(x);
}

f32 rq_tan(f32 x)  {
    return tanf(x);
}

f32 rq_acos(f32 x) {
    return acosf(x);
}

f32 rq_sqrt(f32 x) {
    return sqrtf(x);
}

f32 rq_abs(f32 x)  {
    return fabsf(x);
} 

i32 random_int() {
    if (rand_seeded) {
        srand((u32)platform_get_absolute_time());
        rand_seeded = TRUE;
    }
    return rand();
}

i32 random_in_range_int(i32 min, i32 max) {
    if (rand_seeded) {
        srand((u32)platform_get_absolute_time());
        rand_seeded = TRUE;
    }
    return (rand() % (max - min + 1)) + min;
}

f32 random_float() {
    return (float)random_int() / (f32)RAND_MAX;
}

f32 random_in_range_float(f32 min, f32 max) {
    return min + ((float)random_int() / ((f32)RAND_MAX / (max - min)));
}
