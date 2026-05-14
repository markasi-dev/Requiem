#include <core/logger.h>
#include <core/asserts.h>

//TODO: test
#include <platform/platform.h>

int main(void){
    RQ_FATAL("A test message: %f", 3.14f);
    RQ_ERROR("A test message: %f", 3.14f);
    RQ_WARN("A test message: %f", 3.14f);
    RQ_INFO("A test message: %f", 3.14f);
    RQ_DEBUG("A test message: %f", 3.14f);
    RQ_TRACE("A test message: %f", 3.14f);

    platform_state state;
    platform_config config;
    config.application_name = "Requiem Engine | Testbed";
    config.x = 100;
    config.y = 100;
    config.width = 1280;
    config.height = 720;
    if (platform_startup(&state, config)) {
        while (TRUE) {
            platform_pump_messages(&state);
        }
    }
    platform_shutdown(&state);

    return 0;
}