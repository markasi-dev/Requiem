#include "rq_memory.h"

#include "logger.h"
#include "platform/platform.h"
#include "core/rq_string.h"

// TODO: Custom string lib
#include <string.h>
#include <stdio.h>

struct memory_stats {
    u64 total_allocated;
    u64 tagged_allocations[MEMORY_TAG_MAX_TAGS];
};

static const char* memory_tag_strings[MEMORY_TAG_MAX_TAGS] = {
    "UNKNOWN          ",
    "ARRAY            ",
    "DARRAY           ",
    "DICT             ",
    "RING_QUEUE       ",
    "BST              ",
    "STRING           ",
    "APPLICATION      ",
    "JOB              ",
    "TEXTURE          ",
    "MATERIAL_INSTANCE",
    "RENDERER         ",
    "GAME             ",
    "TRANSFORM        ",
    "ENTITY           ",
    "ENTITY_NODE      ",
    "SCENE            "};

static struct memory_stats stats;

void initialize_memory() {
    platform_zero_memory(&stats, sizeof(stats));
    RQ_INFO("Initialized Memory.");
}

void shutdown_memory() {
}

void* rq_allocate(u64 size, memory_tag tag) {
    if (tag == MEMORY_TAG_UNKNOWN) {
        RQ_WARN("rq_allocate called using MEMORY_TAG_UNKNOWN. Please re-class this allocation.");
    };

    stats.total_allocated += size;
    stats.tagged_allocations[tag] += size;

    // TODO: Memory allignment.
    void* block = platform_allocate(size, FALSE);
    platform_zero_memory(block, size); 
    return block;
}

void rq_free(void* block, u64 size, memory_tag tag) {
    if (tag == MEMORY_TAG_UNKNOWN) {
        RQ_WARN("rq_free called using MEMORY_TAG_UNKNOWN. Please re-class this allocation.");
    }

    stats.total_allocated -= size;
    stats.tagged_allocations[tag] -= size;

    //TODO: Memory allignment
    platform_free(block, FALSE);
}

void* rq_zero_memory(void* block, u64 size) {
    return platform_zero_memory(block, size);
}

void* rq_copy_memory(void* dest, const void* source, u64 size) {
    return platform_copy_memory(dest, source, size);
}

void* rq_set_memory(void* dest, i32 value, u64 size) {
    return platform_set_memory(dest, value, size);
}

char* get_memory_usage_string() {
    const u64 gib = 1024 * 1024 * 1024;
    const u64 mib = 1024 * 1024;
    const u64 kib = 1024;

    char buffer[8000] = "System memory use (tagged):\n";
    u64 offset = string_length(buffer);
    for (u32 i = 0; i < MEMORY_TAG_MAX_TAGS; i++) {
        char unit[4] = "XiB";
        float amount = 1.0f;
        if (stats.tagged_allocations[i] >= gib) { // It's Gb.
            unit[0] = 'G';
            amount = stats.tagged_allocations[i] / (float)gib;
        } else if (stats.tagged_allocations[i] >= mib) { // It's Mb.
            unit[0] = 'M';
            amount = stats.tagged_allocations[i] / (float)mib;
        } else if (stats.tagged_allocations[i] >= kib) { // It's Kb.
            unit[0] = 'K';
            amount = stats.tagged_allocations[i] / (float)kib;
        } else { // It's bytes.
            unit[0] = 'B';
            unit[1] = 0;
            amount = (float)stats.tagged_allocations[i];
        }

        i32 length = snprintf(buffer + offset, 8000, "  %s: %.2f%s\n", memory_tag_strings[i], amount, unit);
        offset += length;
    }
    char* out_string = string_duplicate(buffer);
    return out_string;
}