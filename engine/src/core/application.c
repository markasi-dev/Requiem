#include "application.h"
#include "logger.h"

#include "platform/platform.h"
#include "core/rq_memory.h"

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

b8 application_create(game* game_inst) {
    if (initialized) {
        RQ_ERROR("application_create called more than once.");
        return FALSE;
    }

    app_state.game_inst = game_inst;

    // Initialize subsystems.
    initialize_logging();

    app_state.is_running = TRUE;
    app_state.is_suspended = FALSE;
    
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
        }
    }

    // Make SURE its false.
    app_state.is_running = FALSE;

    platform_shutdown(&app_state.platform);

    return TRUE;
}