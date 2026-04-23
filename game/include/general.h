#ifndef GENERAL_H
#define GENERAL_H

#include <stdio.h>
#include <math.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <assert.h>

// Typedef for precise bit counts
typedef uint64_t u64;
typedef uint32_t u32;
typedef uint16_t u16;
typedef uint8_t  u8;

typedef int64_t  s64;
typedef int32_t  s32;
typedef int16_t  s16; 
typedef int8_t   s8;

typedef int32_t bool32; // This is a custom bool that doesn't force non zero values to be one.

typedef float  float32;
typedef double float64;

const float32 PI_32 = 3.14159265359f;

#define kilobytes(value) ((value) * 1024ULL)
#define megabytes(value) ((value) * 1024ULL * 1024ULL)
#define gigabytes(value) ((value) * 1024ULL * 1024ULL * 1024ULL)
#define terabytes(value) ((value) * 1024ULL * 1024ULL * 1024ULL * 1024ULL)

#define array_count(array) ( sizeof(array) / sizeof((array)[0]) )
u32 truncate_u64_to_u32(u64 value);

// General arena usage.
struct Arena {
	u8 *base;
	size_t size;
	size_t used;
};

size_t align_forward(size_t ptr, size_t align);

void *arena_allocate(Arena *a, size_t size, size_t align = 8);
void  arena_init(Arena *a, size_t size);
void  arena_init_with_backing_memory(Arena *a, void *memory, size_t size);
void  arena_reset(Arena *a);
void  arena_free(Arena *a);

#define zero_struct(instance) memset(&(instance), 0, sizeof(instance))
#define push_struct_to_arena(arena, type) (type *)arena_allocate(arena, sizeof(type), alignof(type))
#define push_array_to_arena(arena, type, count) (type *)arena_allocate(arena, sizeof(type) * (count), \
																	   alignof(type))

// Dynamic Array stuff using C++ since I don't want to use vectors...
template <typename T>
struct Array {
	T *data;
	size_t count;
	size_t capacity;
};

template <typename T>
void array_init(Array<T> *array, Arena *arena, size_t capacity) {
	assert(arena->base != nullptr);

	array->data     = (T *)arena_allocate(arena, sizeof(T) * capacity, alignof(T));
	array->count    = 0;
	array->capacity = capacity;
}

template <typename T>
void array_push(Array<T> *array, T value) {
	assert(array->count < array->capacity);
	array->data[array->count++] = value;
}

template <typename T>
T *array_get_at_index(Array<T> *array, size_t index) {
	assert(index < array->count);
	return &array->data[index];
}

#endif
