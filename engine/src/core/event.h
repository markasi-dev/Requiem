#pragma once

#include "defines.h"

typedef struct {
    // Maximum of 128 bytes
    union {
        i64 i64[2];
        u64 u64[2];
        f64 f64[2];

        i32 i32[4];
        u32 u32[4];
        f32 f32[4];

        i16 i16[8];
        u16 u16[8];

        i8 i8[16];
        u8 u8[16];

        char c[16];
    } data;
} event_context;

// Should return true if handled.
typedef b8 (*PFN_on_event)(u16 code, void* sender, void* listener_inst, event_context data);

void event_system_initialize();
void event_system_shutdown();

RAPI b8 event_register(u16 code, void* listener, PFN_on_event on_event);

RAPI b8 event_unregister(u16 code, void* listener, PFN_on_event on_event);

RAPI b8 event_fire(u16 code, void* sender, event_context context);

// System internal event codes. Application should use codes beyond 255.
typedef enum {
    // Shuts the appliction down on the next frame
    EVENT_CODE_APPLICATION_QUIT = 0x01,

    // Keyboard key pressed.
    EVENT_CODE_KEY_PRESSED = 0x02,

    // Keyboard key released.
    EVENT_CODE_KEY_RELEASED = 0x03,
    
    // Mouse button pressed.
    EVENT_CODE_BUTTON_PRESSED = 0x04,

    // Mouse button released.
    EVENT_CODE_BUTTON_RELEASED = 0x05,

    // Mouse moved.
    EVENT_CODE_MOUSE_MOVED = 0x06,

    // Mouse wheel wheeled.
    EVENT_CODE_MOUSE_WHEELED = 0x07,

    // Resized/resolution changed from the OS
    EVENT_CODE_RESIZED = 0x08,

    MAX_EVENT_CODES = 0xff
} system_event_code;