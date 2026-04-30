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
	assert(a->base);

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

// This makes an allocation on the arena to create the string.
String string_create(Arena *arena, const char *str) {
	assert(arena);
	assert(str);

	u64 length = 0;
	while (str[length]) {
		length++;
	}

	String result;
	result.data = (char *)arena_allocate(arena, (length + 1), alignof(char));
	result.length = length;
	if (result.data) {
		memcpy(result.data, str, length);
		result.data[length] = '\0';
	}
	
	return result;
}

// This is simply a pointer + length of a string that already exists.
String string_view(char *data, u64 length) {
	assert(data);
	String result;
	result.data = data;
	result.length = length;
	
	return result;
}

String string_concat(Arena *arena, String a, String b) {
	assert(arena);

	u64 total_length = a.length + b.length;
	char *data = (char *)arena_allocate(arena, (total_length + 1), alignof(char));
	assert(data);

	memcpy(data, a.data, a.length);
	memcpy((data + a.length), b.data, b.length);
	data[total_length] = '\0';

	String result;
	result.data = data;
	result.length = total_length;
	return result;
}

String string_slice(String str, u64 start, u64 end) {
	assert(start <= end);
	assert(end <= str.length);

	String result;
	result.data = str.data + start;
	result.length = end - start;
	return result;
}

String string_format(Arena *arena, const char *fmt, ...) {
	assert(arena);
	assert(fmt);

	va_list args;

	// Measure first, then write.
	va_start(args, fmt);
	int needed = vsnprintf(nullptr, 0, fmt, args);
	va_end(args);

	if (needed < 0) return {};

	char *data = (char *)arena_allocate(arena, (size_t)(needed + 1), alignof(char));
	if (!data) return {};

	va_start(args, fmt);
	vsnprintf(data, (size_t)(needed + 1), fmt, args);
	va_end(args);

	String result;
	result.data = data;
	result.length = (u64)needed;
	return result;
}

bool string_comp(String a, String b) {
	if (a.length != b.length) return false;
	return memcmp(a.data, b.data, a.length) == 0;
}

const char *string_to_cstr(String str) {
	return (const char *)str.data;
}
