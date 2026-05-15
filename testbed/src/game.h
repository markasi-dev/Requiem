#pragma once

#include <defines.h>
#include <game_types.h>

typedef struct game_state {
    f32 delta_time;
} game_state;

b8 game_initialize(game* game_inst);

b8 game_update(struct game* game_inst, f32 deltaTime);

b8 game_render(struct game* game_inst, f32 deltaTime);

void game_resize(struct game* game_inst, u32 width, u32 height);
