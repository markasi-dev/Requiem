#pragma once

#include "defines.h"

struct platform_state;
struct vulkan_context;

b8 platform_create_vulkan_surface(
    struct platform_state* plat_state,
    struct vulkan_context* context);

/**
 * I'm totally not putting a comment to make this look less uncannily 
 * empty! Right...? :p
 */

void platform_get_required_extension_names(const char*** names_darray);