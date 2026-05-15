#include "game.h"

#include <core/logger.h>

b8 game_initialize(game* game_inst) {
    RQ_DEBUG("game_initialize() called.")
    return TRUE;
}

b8 game_update(struct game* game_inst, f32 deltaTime) {
    return TRUE;
}

b8 game_render(struct game* game_inst, f32 deltaTime) {
    return TRUE;
}

void game_resize(struct game* game_inst, u32 width, u32 height) {
}