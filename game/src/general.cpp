#include "general.h"
#include "raylib.h"

#include <stdlib.h>
#include <assert.h>

u32 truncate_u64_to_u32(u64 value) {
	// @TODO: Create defines for maximum and minimum values.
	assert(value <= 0xFFFFFFFF);
	u32 result = (u32)value;
	return result;
}

size_t align_forward(size_t ptr, size_t align) {
	assert((align & (align - 1)) == 0); // Align must be a power of 2.
	
	size_t modulo = ptr & (align - 1);
	if (modulo != 0) ptr += (align - modulo);
	return ptr;
}

void *arena_allocate(Arena *a, size_t size, size_t align) {
    assert(a);
    assert(a->base);

    size_t current = (size_t)a->base + a->used;
    size_t aligned  = align_forward(current, align);

    size_t new_used = (aligned - (size_t)a->base) + size;

    if (new_used > a->size) {
        return nullptr;
    }

    void *result = (void *)aligned;
    a->used = new_used;
    return result;
}

// @TODO: Use platform-specific memory allocation to avoid calloc() overhead.
void arena_init(Arena *a, size_t size) {
	a->base = (u8 *)MemAlloc(size); // Raylib calls out to calloc() and zeroes it out.
	memset(a->base, 0, size);       // Just in case.

	a->size = size;
	a->used = 0;
}

void arena_init_with_backing_memory(Arena *a, void *memory, size_t size) {
	a->base = (u8 *)memory;
	a->size = size;
	a->used = 0;
}

void arena_reset(Arena *a) {
	a->used = 0;
}

void arena_free(Arena *a) {
	MemFree(a->base);
	a->base = nullptr;
	a->size = 0;
	a->used = 0;
}
