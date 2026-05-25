#pragma once

#include "defines.h"
#include "math_types.h"
#include "core/rq_memory.h"

#define RQ_PI 3.14159265358979323846f
#define RQ_PI_2 (2.0f * RQ_PI)
#define RQ_HALF_PI (0.5f * RQ_PI)
#define RQ_QUARTER_PI (0.25f * RQ_PI)

#define RQ_ONE_OVER_PI (1.0f / RQ_PI)
#define RQ_ONE_OVER_TWO_PI (1.0f / RQ_PI_2)

#define RQ_DEG2RAD_MULTIPLIER (RQ_PI / 180.0f)
#define RQ_RAD2DEG_MULTIPLIER (180.0f / RQ_PI)

#define RQ_SEC_TO_MS_MULTIPLIER 1000.0f

#define RQ_MS_TO_SEC_MULTIPLIER 0.001f

// Just a big fucking number
#define RQ_INFINITY 1e30f

// Just the smallest positive fucking number where 1.0 + FLOAT_EPSILON != 0
#define RQ_FLOAT_EPSILON 1.192092896e-07f

//------------------------------------
// GENERAL MATH FUNCTIOINS
//------------------------------------
RAPI f32 rq_sin(f32 x);
RAPI f32 rq_cos(f32 x);
RAPI f32 rq_tan(f32 x);
RAPI f32 rq_acos(f32 x);
RAPI f32 rq_sqrt(f32 x);
RAPI f32 rq_abs(f32 x);

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
    return rq_sqrt(vec2_length_squared(vector));
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
    if (rq_abs(a.x - b.x) > tolerance) {
        return FALSE;
    }

    if (rq_abs(a.y - b.y) > tolerance) {
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
    return (vec3){0.0f, 0.0f, 0.0f};
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
    return rq_sqrt(vec3_length_squared(vector));
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
    if (rq_abs(a.x - b.x) > tolerance) {
        return FALSE;
    }

    if (rq_abs(a.y - b.y) > tolerance) {
        return FALSE;
    }

    if (rq_abs(a.z - b.z) > tolerance) {
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
    return rq_sqrt(vec4_length_squared(vector));
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

RQ_INLINE mat4 mat4_identity() {
    mat4 out_matrix;
    rq_zero_memory(out_matrix.data, sizeof(f32) * 16);
    out_matrix.data[0] = 1.0f;
    out_matrix.data[5] = 1.0f;
    out_matrix.data[10] = 1.0f;
    out_matrix.data[15] = 1.0f;
    return out_matrix;
}

RQ_INLINE mat4 mat4_mul(mat4 a, mat4 b) {
    mat4 out_matrix = mat4_identity();

    const f32* m1_ptr = a.data;
    const f32* m2_ptr = b.data;
    f32* dst_ptr = out_matrix.data;

    for (i32 i = 0; i < 4; ++i) {
        for (i32 j = 0; j < 4; ++j) {
            *dst_ptr = 
                m1_ptr[0] * m2_ptr[0 + j] +
                m1_ptr[1] * m2_ptr[4 + j] +
                m1_ptr[2] * m2_ptr[8 + j] +
                m1_ptr[3] * m2_ptr[12 + j];
            dst_ptr++;
        }
        m1_ptr += 4;
    }
    return out_matrix;
}

RQ_INLINE mat4 mat4_orthographic(f32 left, f32 right, f32 bottom, f32 top, f32 near_clip, f32 far_clip) {
    mat4 out_matrix = mat4_identity();

    f32 lr = 1.0f / (left - right);
    f32 bt = 1.0f / (bottom - top);
    f32 nf = 1.0f / (near_clip - far_clip);

    out_matrix.data[0] = -2.0f * lr;
    out_matrix.data[5] = -2.0f * bt;
    out_matrix.data[10] = 2.0f * nf;
    
    out_matrix.data[12] = (left + right) * lr;
    out_matrix.data[13] = (top + bottom) * bt;
    out_matrix.data[14] = (far_clip + near_clip) * nf;
    return out_matrix;
}

RQ_INLINE mat4 mat4_perspective(
    f32 fov_radians,
    f32 aspect_ratio,
    f32 near_clip,
    f32 far_clip) {

    f32 half_tan_fov = rq_tan(fov_radians * 0.5f);

    mat4 out_matrix;
    rq_zero_memory(out_matrix.data, sizeof(f32) * 16);

    out_matrix.data[0] = 1.0f / (aspect_ratio * half_tan_fov);
    out_matrix.data[5] = 1.0f / half_tan_fov;

    out_matrix.data[10] = -(far_clip + near_clip) / (far_clip - near_clip);
    out_matrix.data[11] = -1.0f;

    out_matrix.data[14] =
        -(2.0f * far_clip * near_clip) /
        (far_clip - near_clip);

    return out_matrix;
}

RQ_INLINE mat4 mat4_look_at(vec3 position, vec3 target, vec3 up) {
    mat4 out_matrix;
    vec3 z_axis;
    z_axis.x = target.x - position.x;
    z_axis.y = target.y - position.y;
    z_axis.z = target.z - position.z;

    z_axis = vec3_normalized(z_axis);
    vec3 x_axis = vec3_normalized(vec3_cross(z_axis, up));
    vec3 y_axis = vec3_cross(x_axis, z_axis);

    out_matrix.data[0] = x_axis.x;
    out_matrix.data[1] = y_axis.x;
    out_matrix.data[2] = -z_axis.x;
    out_matrix.data[3] = 0;
    out_matrix.data[4] = x_axis.y;
    out_matrix.data[5] = y_axis.y;
    out_matrix.data[6] = z_axis.y;
    out_matrix.data[7] = 0;
    out_matrix.data[8] = x_axis.z;
    out_matrix.data[9] = y_axis.z;
    out_matrix.data[10] = z_axis.z;
    out_matrix.data[11] = 0;
    out_matrix.data[12] = -vec3_dot(x_axis, position);
    out_matrix.data[13] = -vec3_dot(y_axis, position);
    out_matrix.data[14] = vec3_dot(z_axis, position);
    out_matrix.data[15] = 1.0f;

    return out_matrix;
}

RQ_INLINE mat4 mat4_transpose(mat4 matrix) {
    mat4 out_matrix = mat4_identity();
    out_matrix.data[0] = matrix.data[0];
    out_matrix.data[1] = matrix.data[4];
    out_matrix.data[2] = matrix.data[8];
    out_matrix.data[3] = matrix.data[12];
    out_matrix.data[4] = matrix.data[1];
    out_matrix.data[5] = matrix.data[5];
    out_matrix.data[6] = matrix.data[9];
    out_matrix.data[7] = matrix.data[13];
    out_matrix.data[8] = matrix.data[2];
    out_matrix.data[9] = matrix.data[6];
    out_matrix.data[10] = matrix.data[10];
    out_matrix.data[11] = matrix.data[14];
    out_matrix.data[12] = matrix.data[3];
    out_matrix.data[13] = matrix.data[7];
    out_matrix.data[14] = matrix.data[11];
    out_matrix.data[15] = matrix.data[15];
    return out_matrix;
}

RQ_INLINE mat4 mat4_inverse(mat4 matrix) {
    const f32* m = matrix.data;

    f32 t0 = m[10] * m[15];
    f32 t1 = m[14] * m[11];
    f32 t2 = m[6] * m[15];
    f32 t3 = m[14] * m[7];
    f32 t4 = m[6] * m[11];
    f32 t5 = m[10] * m[7];
    f32 t6 = m[2] * m[15];
    f32 t7 = m[14] * m[3];
    f32 t8 = m[2] * m[11];
    f32 t9 = m[10] * m[3];
    f32 t10 = m[2] * m[7];
    f32 t11 = m[6] * m[3];
    f32 t12 = m[8] * m[13];
    f32 t13 = m[12] * m[9];
    f32 t14 = m[4] * m[13];
    f32 t15 = m[12] * m[5];
    f32 t16 = m[4] * m[9];
    f32 t17 = m[8] * m[5];
    f32 t18 = m[0] * m[13];
    f32 t19 = m[12] * m[1];
    f32 t20 = m[0] * m[9];
    f32 t21 = m[8] * m[1];
    f32 t22 = m[0] * m[5];
    f32 t23 = m[4] * m[1];

    mat4 out_matrix;
    f32* o = out_matrix.data;

    o[0] = (t0 * m[5] + t3 * m[9] + t4 * m[13]) - (t1 * m[5] + t2 * m[9] + t5 * m[13]);
    o[1] = (t1 * m[1] + t6 * m[9] + t9 * m[13]) - (t0 * m[1] + t7 * m[9] + t8 * m[13]);
    o[2] = (t2 * m[1] + t7 * m[5] + t10 * m[13]) - (t3 * m[1] + t6 * m[5] + t11 * m[13]);
    o[3] = (t5 * m[1] + t8 * m[5] + t11 * m[9]) - (t4 * m[1] + t9 * m[5] + t10 * m[9]);

    f32 d = 1.0f / (m[0] * o[0] + m[4] * o[1] + m[8] * o[2] + m[12] * o[3]);

    o[0] = d * o[0];
    o[1] = d * o[1];
    o[2] = d * o[2];
    o[3] = d * o[3];
    o[4] = d * ((t1 * m[4] + t2 * m[8] + t5 * m[12]) - (t0 * m[4] + t3 * m[8] + t4 * m[12]));
    o[5] = d * ((t0 * m[0] + t7 * m[8] + t8 * m[12]) - (t1 * m[0] + t6 * m[8] + t9 * m[12]));
    o[6] = d * ((t3 * m[0] + t6 * m[4] + t11 * m[12]) - (t2 * m[0] + t7 * m[4] + t10 * m[12]));
    o[7] = d * ((t4 * m[0] + t9 * m[4] + t10 * m[8]) - (t5 * m[0] + t8 * m[4] + t11 * m[8]));
    o[8] = d * ((t12 * m[7] + t15 * m[11] + t16 * m[15]) - (t13 * m[7] + t14 * m[11] + t17 * m[15]));
    o[9] = d * ((t13 * m[3] + t18 * m[11] + t21 * m[15]) - (t12 * m[3] + t19 * m[11] + t20 * m[15]));
    o[10] = d * ((t14 * m[3] + t19 * m[7] + t22 * m[15]) - (t15 * m[3] + t18 * m[7] + t23 * m[15]));
    o[11] = d * ((t17 * m[3] + t20 * m[7] + t23 * m[11]) - (t16 * m[3] + t21 * m[7] + t22 * m[11]));
    o[12] = d * ((t14 * m[10] + t17 * m[14] + t13 * m[6]) - (t16 * m[14] + t12 * m[6] + t15 * m[10]));
    o[13] = d * ((t20 * m[14] + t12 * m[2] + t19 * m[10]) - (t18 * m[10] + t21 * m[14] + t13 * m[2]));
    o[14] = d * ((t18 * m[6] + t23 * m[14] + t15 * m[2]) - (t22 * m[14] + t14 * m[2] + t19 * m[6]));
    o[15] = d * ((t22 * m[10] + t16 * m[2] + t21 * m[6]) - (t20 * m[6] + t23 * m[10] + t17 * m[2]));

    return out_matrix;
}

RQ_INLINE mat4 mat4_translation(vec3 position) {
    mat4 out_matrix = mat4_identity();
    out_matrix.data[12] = position.x;
    out_matrix.data[13] = position.y;
    out_matrix.data[14] = position.z;
    return out_matrix;
}

RQ_INLINE mat4 mat4_scale(vec3 scale) {
    mat4 out_matrix = mat4_identity();
    out_matrix.data[0] = scale.x;
    out_matrix.data[5] = scale.y;
    out_matrix.data[10] = scale.z;
    return out_matrix;
}

RQ_INLINE mat4 mat4_euler_x(f32 angle_radians) {
    mat4 out_matrix = mat4_identity();
    f32 c = rq_cos(angle_radians);
    f32 s = rq_sin(angle_radians);

    out_matrix.data[5] = c;
    out_matrix.data[6] = s;
    out_matrix.data[9] = -s;
    out_matrix.data[10] = c;
    return out_matrix;
}

RQ_INLINE mat4 mat4_euler_y(f32 angle_radians) {
    mat4 out_matrix = mat4_identity();
    f32 c = rq_cos(angle_radians);
    f32 s = rq_sin(angle_radians);

    out_matrix.data[0] = c;
    out_matrix.data[2] = -s;
    out_matrix.data[8] = s;
    out_matrix.data[10] = c;
    return out_matrix;
}

RQ_INLINE mat4 mat4_euler_z(f32 angle_radians) {
    mat4 out_matrix = mat4_identity();
    f32 c = rq_cos(angle_radians);
    f32 s = rq_sin(angle_radians);

    out_matrix.data[0] = c;
    out_matrix.data[1] = s;
    out_matrix.data[4] = -s;
    out_matrix.data[5] = c;
    return out_matrix;
}

RQ_INLINE mat4 mat4_euler_xyz(f32 x_radians, f32 y_radians, f32 z_radians) {
    mat4 rx = mat4_euler_x(x_radians);
    mat4 ry = mat4_euler_y(y_radians);
    mat4 rz = mat4_euler_z(z_radians); 
    mat4 out_matrix = mat4_mul(rx, ry);
    out_matrix = mat4_mul(out_matrix, rz);
    return out_matrix;
}

RQ_INLINE vec3 mat4_forward(mat4 matrix) {
    vec3 forward;
    forward.x = -matrix.data[2];
    forward.y = -matrix.data[6];
    forward.z = -matrix.data[10];
    vec3_normalize(&forward);
    return forward;
}

RQ_INLINE vec3 mat4_backward(mat4 matrix) {
    vec3 backward;
    backward.x = matrix.data[2];
    backward.y = matrix.data[6];
    backward.z = matrix.data[10];
    vec3_normalize(&backward);
    return backward;
}


RQ_INLINE vec3 mat4_up(mat4 matrix) {
    vec3 up;
    up.x = matrix.data[1];
    up.y = matrix.data[5];
    up.z = matrix.data[9];
    vec3_normalize(&up);
    return up;
}

RQ_INLINE vec3 mat4_down(mat4 matrix) {
    vec3 down;
    down.x = -matrix.data[1];
    down.y = -matrix.data[5];
    down.z = -matrix.data[9];
    vec3_normalize(&down);
    return down;
}

RQ_INLINE vec3 mat4_left(mat4 matrix) {
    vec3 right;
    right.x = -matrix.data[0];
    right.y = -matrix.data[4];
    right.z = -matrix.data[8];
    vec3_normalize(&right);
    return right;
}

RQ_INLINE vec3 mat4_right(mat4 matrix) {
    vec3 left;
    left.x = matrix.data[0];
    left.y = matrix.data[4];
    left.z = matrix.data[8];
    vec3_normalize(&left);
    return left;
}

//------------------------------------
// QUATERNIONS
//------------------------------------

RQ_INLINE quat quat_identity() {
    return (quat){0, 0, 0, 1.0f};
}

RQ_INLINE f32 quat_normal(quat q) {
    return rq_sqrt(
        q.x * q.x + 
        q.y * q.y +
        q.z * q.z +
        q.w * q.w);
}

RQ_INLINE quat quat_normalize(quat q) {
    f32 normal = quat_normal(q);
    return (quat){
        q.x / normal,
        q.y / normal,
        q.z / normal,
        q.w / normal};
}

RQ_INLINE quat quat_conjugate(quat q) {
    return (quat) {
        -q.x,
        -q.y,
        -q.z,
         q.w};
}

RQ_INLINE quat quat_inverse(quat q) {
    return quat_normalize(quat_conjugate(q));
}

RQ_INLINE quat quat_mul(quat a, quat b) {
    quat out_quaternion;

    out_quaternion.x = a.x * b.w +
                       a.y * b.z -
                       a.z * b.y +
                       a.w * b.x;

    out_quaternion.y = -a.x * b.z +
                       a.y * b.w +
                       a.z * b.x +
                       a.w * b.y;

    out_quaternion.z = a.x * b.y -
                       a.y * b.x +
                       a.z * b.w +
                       a.w * b.z;

    out_quaternion.w = -a.x * b.x -
                       a.y * b.y -
                       a.z * b.z +
                       a.w * b.w;

    return out_quaternion;
}

RQ_INLINE f32 quat_dot(quat a, quat b) {
    return a.x * b.x +
           a.y * b.y +
           a.z * b.z +
           a.w * b.w;
}

RQ_INLINE mat4 quat_to_mat4(quat q) {
    mat4 out_matrix = mat4_identity();

    quat n = quat_normalize(q);

    out_matrix.data[0] = 1.0f - 2.0f * n.y * n.y - 2.0f * n.z * n.z;
    out_matrix.data[1] = 2.0f * n.x * n.y - 2.0f * n.z * n.w;
    out_matrix.data[2] = 2.0f * n.x * n.z + 2.0f * n.y * n.w;

    out_matrix.data[4] = 2.0f * n.x * n.y + 2.0f * n.z * n.w;
    out_matrix.data[5] = 1.0f - 2.0f * n.x * n.x - 2.0f * n.z * n.z;
    out_matrix.data[6] = 2.0f * n.y * n.z - 2.0f * n.x * n.w;

    out_matrix.data[8] = 2.0f * n.x * n.z - 2.0f * n.y * n.w;
    out_matrix.data[9] = 2.0f * n.y * n.z + 2.0f * n.x * n.w;
    out_matrix.data[10] = 1.0f - 2.0f * n.x * n.x - 2.0f * n.y * n.y;

    return out_matrix;
}


RQ_INLINE quat quat_from_axis_angle(vec3 axis, f32 angle, b8 normalize) {
    const f32 half_angle = 0.5f * angle;
    f32 s = rq_sin(half_angle);
    f32 c = rq_cos(half_angle);

    quat q = (quat){s * axis.x, s * axis.y, s * axis.z, c};
    if (normalize) {
        return quat_normalize(q);
    }
    return q;
}

RQ_INLINE quat quat_slerp(quat q_0, quat q_1, f32 percentage) {
    quat out_quaternion;
    // Only unit quaternions are valid rotations.
    // Normalize to avoid undefined behavior.
    quat v0 = quat_normalize(q_0);
    quat v1 = quat_normalize(q_1);

    // Compute the cosine of the angle between the two vectors.
    f32 dot = quat_dot(v0, v1);

    // If the dot product is negative, slerp won't take
    // the shorter path. Note that v1 and -v1 are equivalent when
    // the negation is applied to all four components. Fix by
    // reversing one quaternion.
    if (dot < 0.0f) {
        v1.x = -v1.x;
        v1.y = -v1.y;
        v1.z = -v1.z;
        v1.w = -v1.w;
        dot = -dot;
    }

    const f32 DOT_THRESHOLD = 0.9995f;
    if (dot > DOT_THRESHOLD) {
        // If the inputs are too close for comfort, linearly interpolate
        // and normalize the result.
        out_quaternion = (quat){
            v0.x + ((v1.x - v0.x) * percentage),
            v0.y + ((v1.y - v0.y) * percentage),
            v0.z + ((v1.z - v0.z) * percentage),
            v0.w + ((v1.w - v0.w) * percentage)};

        return quat_normalize(out_quaternion);
    }

    // Since dot is in range [0, DOT_THRESHOLD], acos is safe
    f32 theta_0 = rq_acos(dot);          // theta_0 = angle between input vectors
    f32 theta = theta_0 * percentage;  // theta = angle between v0 and result
    f32 sin_theta = rq_sin(theta);       // compute this value only once
    f32 sin_theta_0 = rq_sin(theta_0);   // compute this value only once

    f32 s0 = rq_cos(theta) - dot * sin_theta / sin_theta_0;  // == sin(theta_0 - theta) / sin(theta_0)
    f32 s1 = sin_theta / sin_theta_0;

    return (quat){
        (v0.x * s0) + (v1.x * s1),
        (v0.y * s0) + (v1.y * s1),
        (v0.z * s0) + (v1.z * s1),
        (v0.w * s0) + (v1.w * s1)};
}


RQ_INLINE f32 deg_to_rad(f32 degrees) {
    return degrees * RQ_DEG2RAD_MULTIPLIER;
}

RQ_INLINE f32 rad_to_deg(f32 radians) {
    return radians * RQ_RAD2DEG_MULTIPLIER;
}