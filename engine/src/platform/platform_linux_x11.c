#include "platform.h"

#if RQ_PLATFORM_LINUX

#include "core/logger.h"

#include <xcb/xcb.h>
#include <X11/keysym.h>
#include <X11/XKBlib.h> 
#include <X11/Xlib.h>
#include <X11/Xlib-xcb.h>
#include <sys/time.h>

#if _POSIX_C_SOURCE >= 199309L // Checks if Posix is AT THE VERY LEAST POSIX.1b (1993)
    #include <time.h> // nanosleep
#else
    #include <unistd.h> // usleep
#endif  

#include <stdlib.h>
#include <stdio.h>
#include <string.h>  

typedef struct {
    Display *display;
    xcb_connection_t *connection;
    xcb_window_t window;
    xcb_screen_t *screen;
    xcb_atom_t wm_protocols;
    xcb_atom_t wm_delete_win;
} internal_state;

b8 platform_startup(
    platform_state* plat_state,
    i32 x,
    i32 y,
    u32 width,
    u32 height,
    const char* application_name) {
    
    // Create the internal state.
    plat_state->internal_state = malloc(sizeof(internal_state));
    internal_state *state = (internal_state *)plat_state->internal_state;
    
    state->display = XOpenDisplay(NULL); // Connects to the Xlib connection

    // Turn off key-repeats.
    b32 supported;
    XkbSetDetectableAutoRepeat(state->display, TRUE, &supported); // Much much MUCHHH Better than XAutoRepeatOff() since this works on the client
    
    // Retrieve the connection from the display.
    int screen_p = 0;
    state->connection = XGetXCBConnection(state->display); // Gets the underlying XCB connection... THIS FUCKING API IS MORE OUTDATED THAN WINDOWS??!?

    if (xcb_connection_has_error(state->connection)) {
        RQ_FATAL("Failed to connect to X server via Xlib");
        return FALSE;
    }
    
    // Get data from our lovely X server.
    const struct xcb_setup_t *setup = xcb_get_setup(state->connection);

    // Loop through all of the screens using our iterator
    // Basically, if we have multiple screens, it will well... render multiple screens! I think...
    xcb_screen_iterator_t it = xcb_setup_roots_iterator(setup);
    for (i32 s = screen_p; s > 0; s--) {
        xcb_screen_next(&it);
    }

    // After screens have been looped through, asign it.
    state->screen = it.data;

    // Allocate a XID for the window to be created.
    state->window = xcb_generate_id(state->connection);

    // Register event types.
    // XCB_CW_BACK_PIXEL = Filling the window bg color with a single pixel we set later.
    // XCB_CW_EVENT_MASK is required
    u32 event_mask = XCB_CW_BACK_PIXEL | XCB_CW_EVENT_MASK;

    // Listen for keyboard and mouse buttons
    u32 event_values = XCB_EVENT_MASK_BUTTON_PRESS | XCB_EVENT_MASK_BUTTON_RELEASE |
                      XCB_EVENT_MASK_KEY_PRESS | XCB_EVENT_MASK_KEY_RELEASE |
                      XCB_EVENT_MASK_EXPOSURE | XCB_EVENT_MASK_POINTER_MOTION |
                      XCB_EVENT_MASK_STRUCTURE_NOTIFY;
    
    // Values to be sent over XCB (bg colour, events)
    u32 value_list[] = {state->screen->black_pixel, event_values};

    // FINALLY We create the window with this dogshit API.
    // I genuinelly hate this API. WHY DO WE NEED COOKIES???? (well i do know but like... still this is abysmal fart doo doo shit).
    // Tbf i do want a cookie right now. 
    (void)xcb_create_window(
        state->connection,
        XCB_COPY_FROM_PARENT, // depth
        state->window,
        state->screen->root,    // parent
        x,             
        y,           
        width,
        height,
        0,                      // No border
        XCB_WINDOW_CLASS_INPUT_OUTPUT, // set our class up
        state->screen->root_visual,
        event_mask,
        value_list);
 
    // Change the title... Since we have to make a compeletly different function for it...
    xcb_change_property(
        state->connection,
        XCB_PROP_MODE_REPLACE,
        state->window,
        XCB_ATOM_WM_NAME,
        XCB_ATOM_STRING,
        8, // data should be viewed 8 bits at a time
        strlen(application_name),
        application_name);

    // Tell the server to notify when the window manager
    // attempts to destroy the window
    xcb_intern_atom_cookie_t wm_delete_cookie = xcb_intern_atom(
        state->connection,
        0,
        strlen("WM_DELETE_WINDOW"),
        "WM_DELETE_WINDOW");
    xcb_intern_atom_cookie_t wm_protocol_cookie = xcb_intern_atom(
        state->connection,
        0,
        strlen("WM_PROTOCOLS"),
        "WM_PROTOCOLS");
    xcb_intern_atom_reply_t *wm_delete_reply = xcb_intern_atom_reply(
        state->connection,
        wm_delete_cookie,
        NULL);
    xcb_intern_atom_reply_t *wm_protocol_reply = xcb_intern_atom_reply(
        state->connection,
        wm_protocol_cookie,
        NULL);
    state->wm_delete_win = wm_delete_reply->atom;
    state->wm_protocols = wm_protocol_reply->atom;

    xcb_change_property(
        state->connection,
        XCB_PROP_MODE_REPLACE,
        state->window,
        wm_protocol_reply->atom,
        4,
        32,
        1,
        &wm_delete_reply->atom);
    
    // Map the window to the screen. FINALLY!
    xcb_map_window(state->connection, state->window);

    // Flush the stream
    i32 stream_result = xcb_flush(state->connection);
    if (stream_result <= 0) {
        RQ_FATAL("An error occured when flushing the stream: %d", stream_result);
        return FALSE;
    }

    free(wm_protocol_reply);
    free(wm_delete_reply);

    return TRUE;
}

void platform_shutdown(platform_state* plat_state) {
    // COld-cast to the known type.
    internal_state* state = (internal_state *)plat_state->internal_state;

    xcb_destroy_window(state->connection, state->window);
    XCloseDisplay(state->display);
    free(state);
    plat_state->internal_state = NULL;
}

b8 platform_pump_messages(platform_state* plat_state) {
    internal_state* state = (internal_state *)plat_state->internal_state;

    xcb_generic_event_t *event;
    xcb_client_message_event_t *cm;

    b8 quit_flagged = FALSE;

    // Poll for events untill null is returned
    while ((event = xcb_poll_for_event(state->connection)) != NULL) {

        switch(event->response_type & ~0x80) {
            case XCB_KEY_PRESS:
            case XCB_KEY_RELEASE: {
                //TODO: Key press and releases
            } break;
            case XCB_BUTTON_PRESS:
            case XCB_BUTTON_RELEASE: {
                // TODO: Mouse button and releases..
            } break;
            case XCB_MOTION_NOTIFY: {
                // TODO: Mouse movement.
            } break;

            case XCB_CONFIGURE_NOTIFY: {
                // TODO: Resizing
            } break;

            case XCB_CLIENT_MESSAGE: {
                cm = (xcb_client_message_event_t *)event;

                // Window close event
                if (cm->data.data32[0] == state->wm_delete_win) {
                    quit_flagged = TRUE;
                }
            } break;

            default:
                // TODO: Something else happened
                break;
        }

        free(event);
    }

    return !quit_flagged;
}

void* platform_allocate(u64 size, b8 aligned) {
    return malloc(size);
}

void platform_free(void* block, b8 aligned) {
    free(block);
}

void* platform_zero_memory(void* block, u64 size) {
    return memset(block, 0, size);
}

void* platform_copy_memory(void* dest, const void* source, u64 size) {
    return memcpy(dest, source, size);
}

void* platform_set_memory(void* dest, i32 value, u64 size) {
    return memset(dest, value, size);
}

void platform_console_write(const char* message, u8 color) {
    // FATAL, ERROR, WARN, INFO, DEBUG, TRACE
    const char* colour_strings[] = {"0;41", "1;31", "1;33", "1;32", "1;34", "1;30"};
    printf("\033[%sm%s\033[0m", colour_strings[color], message); // now this is fucking weird.
}

void platform_console_write_error(const char* message, u8 color) {
    // FATAL, ERROR, WARN, INFO, DEBUG, TRACE
    const char* colour_strings[] = {"0;41", "1;31", "1;33", "1;32", "1;34", "1;30"};
    fprintf(stderr, "\033[%sm%s\033[0m", colour_strings[color], message); 
}

f64 platform_get_absolute_time() {
    struct timespec now;
    clock_gettime(CLOCK_MONOTONIC, &now);
    return now.tv_sec + now.tv_nsec * 0.000000001;
}

void platform_sleep(u64 ms) {
    #if _POSIX_C_SOURCE >= 199309L
        struct timespec ts;
        ts.tv_sec = ms / 1000;
        ts.tv_nsec = (ms % 1000) * 1000 * 1000;
        nanosleep(&ts, 0);
    #else
        if (ms >= 1000) {
            sleep(ms / 1000);
        }
        usleep((ms % 1000) * 1000);
    #endif
}
#endif