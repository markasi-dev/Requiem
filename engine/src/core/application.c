#include "application.h"
#include "logger.h"

#include "platform/platform.h"
#include "core/rq_memory.h"
#include "core/event.h"
#include "core/input.h"

#include <game_types.h>

// Will grow in size as the engine grows.
typedef struct { 
    game* game_inst;
    b8 is_running;
    b8 is_suspended;
    platform_state platform;
    i16 width; 
    i16 height;
    f64 last_time;
} application_state;

static b8 initialized = FALSE;
static application_state app_state;

// Event handlers
b8 application_on_event(u16 code, void* sender, void* listener_inst, event_context context);
b8 application_on_key(u16 code, void* sender, void* listener_inst, event_context context);

b8 application_create(game* game_inst) {
    if (initialized) {
        RQ_ERROR("application_create called more than once.");
        return FALSE;
    }

    app_state.game_inst = game_inst;

    // Initialize subsystems.
    logging_initialize();
    input_initialize();

    app_state.is_running = TRUE;
    app_state.is_suspended = FALSE;
    
    if (!event_initialize()) {
        RQ_ERROR("Event system failed to initialize. Application will not continue.");
        return FALSE;
    }

    event_register(EVENT_CODE_APPLICATION_QUIT, 0, application_on_event);
    event_register(EVENT_CODE_KEY_PRESSED, 0, application_on_key);
    event_register(EVENT_CODE_KEY_RELEASED, 0, application_on_key);

    if (!platform_startup(
            &app_state.platform, 
            game_inst->app_config.start_pos_x, 
            game_inst->app_config.start_pos_y,
            game_inst->app_config.start_width,
            game_inst->app_config.start_height,
            game_inst->app_config.name)) { 
        return FALSE;
    }

    // Initialize the game.
    if (!app_state.game_inst->initialize(app_state.game_inst)) {
        RQ_FATAL("Game failed to initialize.");
        return FALSE;
    }

    app_state.game_inst->on_resize(app_state.game_inst, app_state.width, app_state.height);

    initialized = TRUE;

    return TRUE;

}
b8 application_run() {
    RQ_INFO(get_memory_usage_string());

    while (app_state.is_running) {
        if (!platform_pump_messages(&app_state.platform)) {
            app_state.is_running = FALSE;
        }

        if (!app_state.is_suspended) {
            if (!app_state.game_inst->update(app_state.game_inst, (f32)0)) {
                RQ_FATAL("Game failed to update. This requires a shutdown.");
                app_state.is_running = FALSE;
                break;
            }

            if (!app_state.game_inst->render(app_state.game_inst, (f32)0)) {
                RQ_FATAL("Game render failed. This requires a shutdown.");
                app_state.is_running = FALSE;
                break;
            }

            input_update(0);
        }
    }

    // Make SURE its false.
    app_state.is_running = FALSE;

    event_unregister(EVENT_CODE_APPLICATION_QUIT, 0, application_on_event);
    event_unregister(EVENT_CODE_KEY_PRESSED, 0, application_on_key);
    event_unregister(EVENT_CODE_KEY_RELEASED, 0, application_on_key);
    event_shutdown();
    input_shutdown();

    platform_shutdown(&app_state.platform);

    return TRUE;
}

b8 application_on_event(u16 code, void* sender, void* listener_inst, event_context context) {
    switch (code) {
        case EVENT_CODE_APPLICATION_QUIT: {
            RQ_INFO("EVENT_CODE_APPLICATION_QUIT Recieved. Shutting down...\n");
            app_state.is_running = FALSE;
            return TRUE;
        }
    }

    return FALSE;
}

b8 application_on_key(u16 code, void* sender, void* listener_inst, event_context context) {
    if (code == EVENT_CODE_KEY_PRESSED) {
        u16 key_code = context.data.u16[0];
        if (key_code == KEY_ESCAPE) {
            //NOTE: We are firing an event to ourselves but hey! There may be other listeners
            event_context data = {};
            event_fire(EVENT_CODE_APPLICATION_QUIT, 0, data);

            // Block anything from processing this.
            return TRUE;
        } else if (key_code == KEY_A) {
            // Example on checking for a key
            RQ_DEBUG("Explicit - 'A' key pressed!");
        } else {
            RQ_DEBUG("'%c' Pressed in window.", key_code);
        }
    } else if (code == EVENT_CODE_KEY_RELEASED) {
        u16 key_code = context.data.u16[0];
        if (key_code == KEY_B) {
            RQ_DEBUG("Explicit - B Key Released!");
        } else {
            RQ_DEBUG("'%c' Released in window.", key_code);
        }
    }

    return FALSE;
}