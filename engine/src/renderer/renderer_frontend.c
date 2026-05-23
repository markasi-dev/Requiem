#include "renderer_frontend.h"
#include "renderer_backend.h"

#include "core/logger.h"
#include "core/rq_memory.h"

// Backend render context
static renderer_backend* backend = 0;

b8 renderer_initialize(const char* application_name, struct platform_state* plat_state) {
    backend = rq_allocate(sizeof(renderer_backend), MEMORY_TAG_RENDERER);
    
    // TODO: Make this configurable
    renderer_backend_create(RENDERER_BACKEND_TYPE_VULKAN, plat_state, backend);

        if (!backend->initialize(backend, application_name, plat_state)) {
        RQ_FATAL("Renderer backend failed to initialize. Killing the application...");
        return FALSE;
    }

    return TRUE;
}

void renderer_shutdown() {
    backend->shutdown(backend);
    rq_free(backend, sizeof(renderer_backend), MEMORY_TAG_RENDERER);
}

b8 renderer_begin_frame(f32 delta_time) {
    return backend->begin_frame(backend, delta_time);
}

b8 renderer_end_frame(f32 delta_time) {
    b8 result = backend->end_frame(backend, delta_time);
    backend->frame_count++;
    return result;
}

void renderer_on_resized(u16 width, u16 height) {
    if (backend) {
        backend->resized(backend, width, height);
    } else {
        RQ_WARN("renderer backend does not exist to accept resize: %i, %i", width, height);
    }
}

b8 renderer_draw_frame(render_packet* packet) {
    if (renderer_begin_frame(packet->delta_time)) {

        // End frame if this failed. it is likely unrecoverable
        b8 result = renderer_end_frame(packet->delta_time); 

        if (!result) {
            RQ_ERROR("renderer_end_frame failed. Application shutting down.")
            return FALSE;
        }
    }

    return TRUE;
}