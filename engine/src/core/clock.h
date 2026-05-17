#pragma once

#include "defines.h"

typedef struct clock {
    f64 start_time;
    f64 elapsed;
} clock;

// Update the clock
void clock_update(clock* clock);

// Start clock. Reset elapsed time.
void clock_start(clock* clock);

// Stop clock. Do not reset elapsed time.
void clock_stop(clock* clock);