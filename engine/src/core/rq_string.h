#pragma once

#include "defines.h"

RAPI u64 string_length(const char* str);

RAPI char* string_duplicate(const char* str);

RAPI b8 strings_equal(const char* str0, const char* str1);
