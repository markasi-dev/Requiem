#include "event.h"

#include "memory/rq_memory.h"
#include "core/logger.h"
#include "containers/darray.h"

typedef struct {
    void* listener;
    PFN_on_event callback;
} registered_event;

typedef struct {
    registered_event* events;
} event_code_entry;

// Hopefully this is enough. HOPEFULLY...
#define MAX_MESSAGE_CODES 16384

// State structure.
typedef struct {
    // Lookup tables for event codes.
    event_code_entry registered[MAX_MESSAGE_CODES];
} event_system_state;

/**
 * Event system internal state
 */
static event_system_state* state_ptr;

void event_system_initialize(u64* memory_requirement, void* state) {
    *memory_requirement = sizeof(event_system_state);
    if (state == 0) {
        return;
    }
    rq_zero_memory(&state, sizeof(state));
    state_ptr = state;
}

void event_system_shutdown() {
    // From the event arrays. And objects pointed to should be destroyed on their own.
    for (u16 i = 0; i < MAX_MESSAGE_CODES; i++) {
        if (state_ptr->registered[i].events != 0) {
            darray_destroy(state_ptr->registered[i].events);
            state_ptr->registered[i].events = 0;
        }
    }
}

b8 event_register(u16 code, void* listener, PFN_on_event on_event) {

    if (state_ptr == FALSE) {
        return FALSE;
    }

    if (state_ptr->registered[code].events == 0) {
        state_ptr->registered[code].events = darray_create(registered_event);
    }

    if (code >= MAX_MESSAGE_CODES) { // I have no clue how the fuck this could happen but hey! Always check.
        RQ_FATAL("YOU HAVE EXCEEDED THE MAX AMOUNT OF MESSAGE CODES! EVENT CODE %u EXCEEDS MAX MESSAGE CODES (%u)", code, MAX_MESSAGE_CODES);
        return FALSE;
    }

    u64 registered_count = darray_length(state_ptr->registered[code].events);
    for (u64 i = 0; i < registered_count; i++) {
        if (state_ptr->registered[code].events[i].listener == listener) {
            RQ_WARN("The events listener is the same listener! This is a duplicate.");
            return FALSE;
        }
    }

    // If at this stage, no duplicated were found, Proceed with registration.
    registered_event event;
    event.listener = listener;
    event.callback = on_event;
    darray_push(state_ptr->registered[code].events, event);

    return TRUE;
}

b8 event_unregister(u16 code, void* listener, PFN_on_event on_event) {
    if (state_ptr == FALSE) {
        return FALSE;
    }

    if (state_ptr->registered[code].events == 0) {
        // TODO: Warn.
        return FALSE;
    }

    if (code >= MAX_MESSAGE_CODES) {
        RQ_FATAL("YOU HAVE EXCEEDED THE MAX AMOUNT OF MESSAGE CODES! EVENT CODE %u EXCEEDS MAX MESSAGE CODES (%u)", code, MAX_MESSAGE_CODES);
        return FALSE;
    }

    u64 registered_count = darray_length(state_ptr->registered[code].events);
    for (u64 i = 0; i < registered_count; i++) {
        registered_event e = state_ptr->registered[code].events[i];
        if (e.listener == listener && e.callback == on_event) {
            // Found it, remove it!
            registered_event popped_event;
            darray_pop_at(state_ptr->registered[code].events, i, &popped_event);
            return TRUE;
        }
    }

    // Not found.
    return FALSE;
}

b8 event_fire(u16 code, void* sender, event_context context) {
    if (state_ptr == FALSE) {
        return FALSE;
    }

    if (state_ptr->registered[code].events == 0) {
        // TODO: Warn.
        return FALSE;
    }

    if (code >= MAX_MESSAGE_CODES) { 
        RQ_FATAL("YOU HAVE EXCEEDED THE MAX AMOUNT OF MESSAGE CODES! EVENT CODE %u EXCEEDS MAX MESSAGE CODES (%u)", code, MAX_MESSAGE_CODES);
        return FALSE;
    }

    u64 registered_count = darray_length(state_ptr->registered[code].events);
    for (u64 i = 0; i < registered_count; i++) {
        registered_event e = state_ptr->registered[code].events[i];
        if (e.callback(code, sender, e.listener, context)) {
            // Message has been handled. Do not send to other listeners.
            return TRUE;
        }
    }

    // Not found. 
    return FALSE;
}