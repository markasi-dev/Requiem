#include "application.h"
#include "logger.h"

#include "platform/platform.h"

#include "memory/rq_memory.h"
#include "memory/linear_allocator.h"

#include "core/event.h"
#include "core/input.h"
#include "core/clock.h"

#include "renderer/renderer_frontend.h"

#include <game_types.h>

typedef struct { 
    game* game_inst;
    b8 is_running;
    b8 is_suspended;
    i16 width; 
    i16 height;
    clock clock;    
    f64 last_time;
    linear_allocator systems_allocator;

    // System states and requirements.
    u64 event_system_memory_requirement;
    void* event_system_state;

    u64 memory_system_memory_requirement;
    void* memory_system_state;

    u64 logging_system_memory_requirement;
    void* logging_system_state;

    u64 input_system_memory_requirement;
    void* input_system_state;

    u64 render_system_memory_requirement;
    void* render_system_state;

} application_state;

static application_state* app_state; 

// Event handlers
b8 application_on_event(u16 code, void* sender, void* listener_inst, event_context context);
b8 application_on_key(u16 code, void* sender, void* listener_inst, event_context context);
b8 application_on_resized(u16 code, void* sender, void* listener_inst, event_context context);

b8 application_create(game* game_inst) {
    if (game_inst->application_state) {
        RQ_ERROR("application_create called more than once.");
        return FALSE;
    }

    game_inst->application_state = rq_allocate(sizeof(application_state), MEMORY_TAG_APPLICATION);
    app_state = game_inst->application_state;
    app_state->game_inst = game_inst;
    app_state->is_running = FALSE;
    app_state->is_suspended = FALSE;

    u64 systems_allocator_total_size = 64 * 1024 * 1024; // 64 MiB
    linear_allocator_create(systems_allocator_total_size, 0, &app_state->systems_allocator);

    // Initialize subsystems.   

    // Events
    event_system_initialize(&app_state->event_system_memory_requirement, 0);
    app_state->event_system_state = linear_allocator_allocate(&app_state->systems_allocator, app_state->event_system_memory_requirement);
    event_system_initialize(&app_state->event_system_memory_requirement, app_state->event_system_state);

    // Memory
    memory_initialize(&app_state->memory_system_memory_requirement, 0);
    app_state->memory_system_state = linear_allocator_allocate(&app_state->systems_allocator, app_state->memory_system_memory_requirement);
    memory_initialize(&app_state->memory_system_memory_requirement, app_state->memory_system_state);

    // Logging
    logging_initialize(&app_state->logging_system_memory_requirement, 0);
    app_state->logging_system_state = linear_allocator_allocate(&app_state->systems_allocator, app_state->logging_system_memory_requirement);
    if (!logging_initialize(&app_state->logging_system_memory_requirement, app_state->logging_system_state)) {
        RQ_ERROR("Failed to initialize logging system; shutting down...");
        return FALSE;
    }

    // Input
    input_initialize(&app_state->input_system_memory_requirement, 0);
    app_state->input_system_state = linear_allocator_allocate(&app_state->systems_allocator, app_state->input_system_memory_requirement);
    input_initialize(&app_state->input_system_memory_requirement, app_state->input_system_state);

    app_state->is_running = TRUE;
    app_state->is_suspended = FALSE;
    
    if (!event_initialize()) {
        RQ_ERROR("Event system failed to initialize. Application will not continue.");
        return FALSE;
    }

    event_register(EVENT_CODE_APPLICATION_QUIT, 0, application_on_event);
    event_register(EVENT_CODE_KEY_PRESSED, 0, application_on_key);
    event_register(EVENT_CODE_KEY_RELEASED, 0, application_on_key);
    event_register(EVENT_CODE_RESIZED, 0, application_on_resized);

    app_state->width  = game_inst->app_config.start_width;
    app_state->height = game_inst->app_config.start_height;

    if (!platform_startup(
            &app_state->platform, 
            game_inst->app_config.start_pos_x, 
            game_inst->app_config.start_pos_y,
            game_inst->app_config.start_width,
            game_inst->app_config.start_height,
            game_inst->app_config.name)) { 
        return FALSE;
    }

    // Renderer startup.
    if (!renderer_initialize(game_inst->app_config.name, &app_state->platform)) {
        RQ_FATAL("Failed to initialize renderer. Killing the application...");
        return FALSE;
    }

    // Initialize the game.
    if (!app_state->game_inst->initialize(app_state->game_inst)) {
        RQ_FATAL("Game failed to initialize.");
        return FALSE;
    }

    return TRUE;

}
b8 application_run() {
    
    RQ_INFO(get_memory_usage_string());

    clock_start(&app_state->clock);
    clock_update(&app_state->clock);
    app_state->last_time = app_state->clock.elapsed;
    f64 running_time = 0;
    u8 frame_count = 0;
    f64 target_frame_seconds = 1.0f / 60;

    while (app_state->is_running) {
        if (!platform_pump_messages(&app_state->platform)) {
            app_state->is_running = FALSE;
        }

        if (!app_state->is_suspended) {
            // Update clock and get delta time
            clock_update(&app_state->clock);
            f64 current_time = app_state->clock.elapsed;
            f64 delta = (current_time - app_state->last_time);
            f64 frame_start_time = platform_get_absolute_time();

            if (!app_state->game_inst->update(app_state->game_inst, (f32)delta)) {
                RQ_FATAL("Game failed to update. This requires a shutdown.");
                app_state->is_running = FALSE;
                break;
            }

            if (!app_state->game_inst->render(app_state->game_inst, (f32)delta)) {
                RQ_FATAL("Game render failed. This requires a shutdown.");
                app_state->is_running = FALSE;
                break;
            }

            render_packet packet;
            packet.delta_time = delta;
            renderer_draw_frame(&packet);

            f64 frame_end_time = platform_get_absolute_time();
            f64 frame_elapsed_time = frame_end_time - frame_start_time;
            running_time += frame_elapsed_time;
            f64 remaining_seconds = target_frame_seconds - frame_elapsed_time;

            if (remaining_seconds > 0) {
                u64 remaining_ms = (remaining_seconds * 1000);

                // If there is time left, give it back to the OS.
                b8 limit_frames = FALSE;
                if (remaining_ms > 0 && limit_frames) {
                    platform_sleep(remaining_ms - 1);
                }

                frame_count++;
            }

            input_update(delta);

            app_state->last_time = current_time;
        }
    }

    // Make SURE its false.
    app_state->is_running = FALSE;

    event_unregister(EVENT_CODE_APPLICATION_QUIT, 0, application_on_event);
    event_unregister(EVENT_CODE_KEY_PRESSED, 0, application_on_key);
    event_unregister(EVENT_CODE_KEY_RELEASED, 0, application_on_key);
    event_shutdown();
    input_shutdown();

    renderer_shutdown();

    platform_shutdown(&app_state->platform);

    memory_shutdown(&app_state->memory_system_state);

    return TRUE;
}

void application_get_framebuffer_size(u32* width, u32* height) {
    *width  = app_state->width;
    *height = app_state->height;
}

b8 application_on_event(u16 code, void* sender, void* listener_inst, event_context context) {
    switch (code) {
        case EVENT_CODE_APPLICATION_QUIT: {
            RQ_INFO("EVENT_CODE_APPLICATION_QUIT Recieved. Shutting down...\n");
            app_state->is_running = FALSE;
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

b8 application_on_resized(u16 code, void* sender, void* listener_inst, event_context context) {
    if (code == EVENT_CODE_RESIZED) {
        u16 width = context.data.u16[0];
        u16 height = context.data.u16[1];

        if (width != app_state->width || height != app_state->height) {
            app_state->width = width;
            app_state->height = height;

            RQ_DEBUG("Window resize: %i, %i", width, height);

            if (width == 0|| height == 0) {
                RQ_INFO("Window minimized, suspending application...");
                app_state->is_suspended = TRUE;
                return TRUE;
            } else {
                if (app_state->is_suspended) {
                    RQ_INFO("Window restored, resuming application...");
                    app_state->is_suspended = FALSE;
                }
                app_state->game_inst->on_resize(app_state->game_inst, width, height);
                renderer_on_resized(width, height);
            }
        }    
    }
    return FALSE;
}