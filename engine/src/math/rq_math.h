#pragma once

#include "defines.h"
#include "math_types.h"

#define PI 3.14159265358979323846f
#define PI_2 2.0f * PI
#define HALF_PI 0.5f * PI
#define QUARTER_PI 0.25f * PI
#define ONE_OVER_PI 1.0f / PI
#define ONE_OVER_TWO_PI 1.0F / PI_2
#define SQRT_2 1.41421356237309504880f
#define SQRT_3 1.73205080756887729352f
#define SQRT_ONE_OVER_TWO 0.70710678118654752440f
#define SQRT_ONE_OVER_THREE 0.57735016918962576450f
#define DEG2RAD_MULTIPLIER PI / 180.0f
#define RAD2DEG_MULTIPLIER 180.0f / PI

#define SEC_TO_MS_MULTIPLIER 1000.0f

#define MS_TO_SEC_MULTIPLIER 0.001f

// Just a big fucking number
#define INFINITY 1e30f

// Just the smallest positive fucking number where 1.0 + FLOAT_EPSILON != 0
#define FLOAT_EPSILON 1.192092896e-07f

//------------------------------------
// GENERAL MATH FUNCTIOINS
//------------------------------------
RAPI f32 sin(f32 x);
RAPI f32 cos(f32 x);
RAPI f32 tan(f32 x);
RAPI f32 acos(f32 x);
RAPI f32 sqrt(f32 x);
RAPI f32 abs(f32 x);

RQ_INLINE b8 is_power_of_2(u64 value) {
    return (value != 0) && ((value & (value - 1)) == 0);
}

RAPI i32 random_int();
RAPI i32 random_in_range_int(i32 min, i32 max);

RAPI f32 random_float();
RAPI f32 random_in_range_float(f32 min, f32 max);

//------------------------------------
// VECTOR 2
//------------------------------------
RQ_INLINE vec2 vec2_create(f32 x, f32 y) {
    vec2 out_vector;
    out_vector.x = x;
    out_vector.y = y;
    return out_vector;
}

RQ_INLINE vec2 vec2_zero() {
    return (vec2){0.0f, 0.0f};
}

RQ_INLINE vec2 vec2_one() {
    return (vec2){1.0f, 1.0f};
}

RQ_INLINE vec2 vec2_up() {
    return (vec2){0.0f, 1.0f};
}

RQ_INLINE vec2 vec2_down() {
    return (vec2){0.0f, -1.0f};
}

RQ_INLINE vec2 vec2_left() {
    return (vec2){-1.0f, 0.0f};
}

RQ_INLINE vec2 vec2_right() {
    return (vec2){1.0f, 0.0f};
}

// Arithmetic
RQ_INLINE vec2 vec2_add(vec2 a, vec2 b) {
    return(vec2){
        a.x + b.x,
        a.y + b.y};
}

RQ_INLINE vec2 vec2_sub(vec2 a, vec2 b) {
    return(vec2){
        a.x - b.x,
        a.y - b.y};
}

RQ_INLINE vec2 vec2_mul(vec2 a, vec2 b) {
    return(vec2){
        a.x * b.x,
        a.y * b.y};
}

RQ_INLINE vec2 vec2_div(vec2 a, vec2 b) {
    return(vec2){
        a.x / b.x,
        a.y / b.y};
}

RQ_INLINE f32 vec2_length_squared(vec2 vector) {
    return vector.x * vector.x + vector.y * vector.y;
}

RQ_INLINE f32 vec2_length(vec2 vector) {
    return sqrt(vec2_length_squared(vector));
}

RQ_INLINE void vec2_normalize(vec2* vector) {
    const f32 length = vec2_length(*vector);
    
    if (length > 0.00001f) {
        vector->x /= length;
        vector->y /= length;
    }
}

RQ_INLINE vec2 vec2_normalized(vec2 vector) {
    vec2_normalize(&vector);
    return vector;
}

RQ_INLINE b8 vec2_compare(vec2 a, vec2 b, f32 tolerance) {
    if (abs(a.x - b.x) > tolerance) {
        return FALSE;
    }

    if (abs(a.y - b.y) > tolerance) {
        return FALSE;
    }

    return TRUE;
}

RQ_INLINE f32 vec2_distance(vec2 a, vec2 b) {
    vec2 d = (vec2){
        a.x - b.x,
        a.y - b.y};
    return vec2_length(d);
}

//------------------------------------
// VECTOR 3
//------------------------------------

RQ_INLINE vec3 vec3_create(f32 x, f32 y, f32 z) {
    return (vec3){x, y, z};
}

RQ_INLINE vec3 vec3_from_vec4(vec4 vector) {
    return (vec3){vector.x, vector.y, vector.z};
}

RQ_INLINE vec4 vec3_to_vec4(vec3 vector, f32 w) {
    return (vec4){vector.x, vector.y, vector.z, w};
}

RQ_INLINE vec3 vec3_zero() {
    return (vec3){0.0f, 0.0f};
}

RQ_INLINE vec3 vec3_one() {
    return (vec3){1.0f, 1.0f, 1.0f};
}

RQ_INLINE vec3 vec3_up() {
    return (vec3){0.0f, 1.0f, 0.0f};
}

RQ_INLINE vec3 vec3_down() {
    return (vec3){0.0f, -1.0f, 0.0f};
}

RQ_INLINE vec3 vec3_left() {
    return (vec3){-1.0f, 0.0f, 0.0f};
}

RQ_INLINE vec3 vec3_right() {
    return (vec3){1.0f, 0.0f, 0.0f};
}

RQ_INLINE vec3 vec3_forward() {
    return (vec3){0.0f, 0.0f, -1.0f};
}

RQ_INLINE vec3 vec3_back() {
    return (vec3){0.0f, 0.0f, 1.0f};
}

// Arithmetic
RQ_INLINE vec3 vec3_add(vec3 a, vec3 b) {
    return(vec3){
        a.x + b.x,
        a.y + b.y,
        a.z + b.z};
}

RQ_INLINE vec3 vec3_sub(vec3 a, vec3 b) {
    return(vec3){
        a.x - b.x,
        a.y - b.y,
        a.z - b.z};
}

RQ_INLINE vec3 vec3_mul(vec3 a, vec3 b) {
    return(vec3){
        a.x * b.x,
        a.y * b.y,
        a.z * b.z};
}

RQ_INLINE vec3 vec3_mul_scalar(vec3 vector_0, f32 scalar) {
    return(vec3){
        vector_0.x * scalar,
        vector_0.y * scalar,
        vector_0.z * scalar
    };
}

RQ_INLINE vec3 vec3_div(vec3 a, vec3 b) {
    return(vec3){
        a.x / b.x,
        a.y / b.y,
        a.z / b.z};
}

RQ_INLINE f32 vec3_length_squared(vec3 vector) {
    return vector.x * vector.x + vector.y * vector.y + vector.z * vector.z;
}

RQ_INLINE f32 vec3_length(vec3 vector) {
    return sqrt(vec3_length_squared(vector));
}

RQ_INLINE void vec3_normalize(vec3* vector) {
    const f32 length = vec3_length(*vector);
    
    if (length > 0.00001f) {
        vector->x /= length;
        vector->y /= length;
        vector->z /= length;
    }
}

RQ_INLINE vec3 vec3_normalized(vec3 vector) {
    vec3_normalize(&vector);
    return vector;
}

RQ_INLINE f32 vec3_dot(vec3 a, vec3 b) {
    f32 p = 0;
    p += a.x * b.x;
    p += a.y * b.y;
    p += a.z * b.z;
    return p;
}

RQ_INLINE vec3 vec3_cross(vec3 a, vec3 b) {
    return (vec3) {
        a.y * b.z - a.z * b.y,
        a.z * b.x - a.x * b.z,
        a.x * b.y - a.y * b.x};
}

RQ_INLINE b8 vec3_compare(vec3 a, vec3 b, f32 tolerance) {
    if (abs(a.x - b.x) > tolerance) {
        return FALSE;
    }

    if (abs(a.y - b.y) > tolerance) {
        return FALSE;
    }

    if (abs(a.z - a.z) > tolerance) {
        return FALSE;
    }

    return TRUE;
}

RQ_INLINE f32 vec3_distance(vec3 a, vec3 b) {
    vec3 d = (vec3){
        a.x - b.x,
        a.y - b.y,
        a.z - b.z};
    return vec3_length(d);
}

//------------------------------------
// VECTOR 4
//------------------------------------
RQ_INLINE vec4 vec4_create(f32 x, f32 y, f32 z, f32 w) {
    vec4 out_vector;
#if defined(RQ_USE_SIMD)
    out_vector.data = _mm_setr_ps(x, y, z, w);
#else
    out_vector.x = x;
    out_vector.y = y;
    out_vector.z = z;
    out_vector.w = w;
#endif
    return out_vector; 
}

RQ_INLINE vec3 vec4_to_vec3(vec4 vector) {
    return (vec3){vector.x, vector.y, vector.z};
}

RQ_INLINE vec4 vec4_from_vec3(vec3 vector, f32 w) {
#if defined(RQ_USE_SIMD)
    vec4 out_vector;
    out_vector.data = _mm_setr_ps(vector.x, vector.y, vector.z, w);
    return out_vector;
#else
    return (vec4){vector.x, vector.y, vector.z, w};
#endif
}

RQ_INLINE vec4 vec4_zero() {
    return (vec4){0.0f, 0.0f, 0.0f,  0.0f};
}

RQ_INLINE vec4 vec4_one() {
    return (vec4){1.0f, 1.0f, 1.0f,  1.0f};
}

RQ_INLINE vec4 vec4_add(vec4 a, vec4 b) {
    vec4 result;
    for (u64 i = 0; i < 4; ++i) {
        result.elements[i] = a.elements[i] + b.elements[i];
    }
    return result;
}

RQ_INLINE vec4 vec4_sub(vec4 a, vec4 b) {
    vec4 result;
    for (u64 i = 0; i < 4; ++i) {
        result.elements[i] = a.elements[i] - b.elements[i];
    }
    return result;
}

RQ_INLINE vec4 vec4_mul(vec4 a, vec4 b) {
    vec4 result;
    for (u64 i = 0; i < 4; ++i) {
        result.elements[i] = a.elements[i] * b.elements[i];
    }
    return result;
}

RQ_INLINE vec4 vec4_div(vec4 a, vec4 b) {
    vec4 result;
    for (u64 i = 0; i < 4; ++i) {
        result.elements[i] = a.elements[i] / b.elements[i];
    }
    return result;
}

RQ_INLINE f32 vec4_length_squared(vec4 vector) {
    return vector.x * vector.x + vector.y * vector.y + vector.z * vector.z + vector.w * vector.w;
} 

RQ_INLINE f32 vec4_length(vec4 vector) {
    return sqrt(vec4_length_squared(vector));
}

RQ_INLINE void vec4_normalize(vec4* vector) {
    const f32 length = vec4_length(*vector);
    
    if (length > 0.00001f) {
        vector->x /= length;
        vector->y /= length;
        vector->z /= length;
        vector->w /= length;
    }
}

RQ_INLINE vec4 vec4_normalized(vec4 vector) {
    vec4_normalize(&vector);
    return vector;
}

RQ_INLINE f32 vec4_dot(
    f32 a0, f32 a1, f32 a2, f32 a3,
    f32 b0, f32 b1, f32 b2, f32 b3) {
    f32 p;
    p = 
        a0 * b0 +
        a1 * b1 +
        a2 * b2 +
        a3 * b3;
    return p;
}