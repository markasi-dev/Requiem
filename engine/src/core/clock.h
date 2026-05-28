#pragma once

#include "defines.h"

typedef struct clock {
    f64 start_time;
    f64 elapsed;
} clock;

// Update the clock
RAPI void clock_update(clock* clock);

// Start clock. Reset elapsed time.
RAPI void clock_start(clock* clock);

// Stop clock. Do not reset elapsed time.
RAPI void clock_stop(clock* clock);