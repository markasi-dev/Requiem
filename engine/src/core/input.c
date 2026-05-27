#include "core/input.h"
#include "core/event.h"
#include "memory/rq_memory.h"
#include "core/logger.h"
#include "core/asserts.h"

// I'm probably going to start using this sort of struct/enum style for readability.
typedef struct keyboard_state {
    b8 keys[256];
} keyboard_state;

typedef struct mouse_state {
    i16 x;
    i16 y;
    u8 buttons[BUTTON_MAX_BUTTONS];
} mouse_state;

typedef struct input_state {
    keyboard_state keyboard_current;
    keyboard_state keyboard_previous;
    mouse_state mouse_current;
    mouse_state mouse_previous;
} input_state; // Ye bird had to cancel her doctors appointment cuz she was sick.

// Internal input state
static b8 initialized = FALSE;
static input_state state = {};

void input_initialize() {
    rq_zero_memory(&state, sizeof(input_state)); // Lets be pedantic
    initialized = TRUE;
    RQ_INFO("Input Subsystem Initialized.");
}

void input_shutdown() {
    // TODO: Add shutdown routines when needed.
    initialized = FALSE;
}

void input_update(f64 deltaTime) {
    if (!initialized) {
        return;
    }

    // Copy current states to previous states.
    rq_copy_memory(&state.keyboard_previous, &state.keyboard_current, sizeof(keyboard_state));
    rq_copy_memory(&state.mouse_previous, &state.mouse_current, sizeof(mouse_state));
}

void input_process_key(keycodes key, b8 pressed) {

    if (key == KEY_LALT) {
        RQ_DEBUG("Left alt pressed.");
    } else if(key == KEY_RALT) {
        RQ_DEBUG("Right alt pressed.");
    } 
    
    if(key == KEY_LCONTROL) {
        RQ_DEBUG("Left control pressed.");
    } else if (key == KEY_RCONTROL) {
        RQ_DEBUG("Right control pressed.");
    } 
    
    if(key == KEY_LSHIFT) {
        RQ_DEBUG("Left shift pressed.");
    } else if (key == KEY_RSHIFT) {
        RQ_DEBUG("Right shift pressed.");
    }

    // Only handle this if the state actually changed
    if (state.keyboard_current.keys[key] != pressed) {
        // Update internal state.
        state.keyboard_current.keys[key] = pressed;

        // Fire off an event for immediate processing
        event_context context;
        context.data.u16[0] = key;
        event_fire(pressed ? EVENT_CODE_KEY_PRESSED : EVENT_CODE_KEY_RELEASED, 0, context);
    }
}

void input_process_button(buttons mb, b8 pressed) {
    // Only handle this if the state actually changed
    if (state.mouse_current.buttons[mb] != pressed) {
        // Update internal state.
        if (mb > BUTTON_MAX_BUTTONS) {
            RQ_ERROR("Mouse button out of range: %u", mb);
            return;
        }

        state.mouse_current.buttons[mb] = pressed;

        // Fire off an event for immediate processing
        event_context context;  
        context.data.u16[0] = mb;
        event_fire(pressed ? EVENT_CODE_BUTTON_PRESSED : EVENT_CODE_BUTTON_RELEASED, 0, context);
    }
}

void input_process_mouse_move(i16 x, i16 y) {
    // Only process if dirty
    if (state.mouse_current.x != x || state.mouse_current.y != y) {
        //NOTE: ENABLE IF DEBUGGING
        //RQ_DEBUG("Mouse pos: %i, %i", x, y);

        // Update internal state.
        state.mouse_current.x = x;
        state.mouse_current.y = y;

        // Fire event.
        event_context context;
        context.data.u16[0] = x;
        context.data.u16[1] = x;
        event_fire(EVENT_CODE_MOUSE_MOVED, 0, context);
    }
}

void input_process_mouse_wheel(i8 z_delta) {
    // NOTE: No internal state to update

    // Fire event
    event_context context;
    context.data.u8[0] = z_delta;
    event_fire(EVENT_CODE_MOUSE_WHEELED, 0, context);
}

RAPI b8 input_is_key_down(keycodes key) {
    if (!initialized) {
        return FALSE;
    }
    return state.keyboard_current.keys[key] == TRUE;
}

RAPI b8 input_is_key_up(keycodes key) {
    if (!initialized) {
        return TRUE;
    }
    return state.keyboard_current.keys[key] == FALSE;
}

RAPI b8 input_was_key_down(keycodes key) {
    if (!initialized) {
        return FALSE;
    }
    return state.keyboard_previous.keys[key] == TRUE;
}

RAPI b8 input_was_key_released(keycodes key) {
    if (!initialized) {
        return TRUE;
    }
    return state.keyboard_previous.keys[key] == FALSE;
}

// mouse input
b8 input_is_mouse_button_down(buttons button) {
    if (!initialized) {
        return FALSE;
    }
    return state.mouse_current.buttons[button] == TRUE;
}

b8 input_is_mouse_button_up(buttons button) {
    if (!initialized) {
        return TRUE;
    }
    return state.mouse_current.buttons[button] == FALSE;
}

b8 input_was_mouse_button_down(buttons button) {
    if (!initialized) {
        return FALSE;
    }
    return state.mouse_previous.buttons[button] == TRUE;
}

b8 input_was_mouse_button_up(buttons button) {
    if (!initialized) {
        return TRUE;
    }
    return state.mouse_previous.buttons[button] == FALSE;
}

void input_get_mouse_position(i32* x, i32* y) {
    if (!initialized) {
        *x = 0;
        *y = 0;
        return;
    }
    *x = state.mouse_current.x;
    *y = state.mouse_current.y;
}

void input_get_previous_mouse_position(i32* x, i32* y) {
    if (!initialized) {
        *x = 0;
        *y = 0;
        return;
    }
    *x = state.mouse_previous.x;
    *y = state.mouse_previous.y;
}

i32 input_get_mouse_position_x() {
    RQ_ASSERT(initialized);
    return state.mouse_current.x;
}

i32 input_get_mouse_position_y() {
    RQ_ASSERT(initialized);
    return state.mouse_current.y;
}

i32 input_get_previous_mouse_position_x() {
    RQ_ASSERT(initialized);
    return state.mouse_previous.x;
}

i32 input_get_previous_mouse_position_y() {
    RQ_ASSERT(initialized);
    return state.mouse_previous.y;
}