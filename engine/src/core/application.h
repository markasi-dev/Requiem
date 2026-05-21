#pragma once

#include "defines.h"

struct game;

// Application configuration.
typedef struct {
    // Window starting position x-axis.
    i16 start_pos_x;

    // Window starting position y-axis.
    i16 start_pos_y;

    // Window starting width x-axis
    i16 start_width;

    // Window starting width y-axis
    i16 start_height;

    // Application name used in windowing
    char* name;
} application_config;

RAPI b8 application_create(struct game* game_inst);
RAPI b8 application_run();

void application_get_framebuffer_size(u32* width, u32* height);