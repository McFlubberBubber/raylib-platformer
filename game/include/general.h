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

struct Font;
struct Vector2;
struct Color;

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

// I didn't want to use std::vector...
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

template <typename T>
void array_unordered_remove(Array<T> *array, size_t index) {
	assert(array);
	assert(index < array->count);

	array->data[index] = array->data[array->count - 1];
	array->count--;
}

template <typename T>
void array_ordered_remove(Array<T> *array, size_t index) {
	assert(array);
	assert(index < array->count);

	size_t num_of_elements_to_shift = array->count - index - 1;
	if (num_of_elements_to_shift > 0) {
		memmove(&array->data[index], &array->data[index + 1], num_of_elements_to_shift * sizeof(T));
	}

	array->count--;
}

// Removes and return the last item in the array.
template <typename T>
T array_pop(Array<T> *array) {
	assert(array);
	assert(array->count > 0);
	return array->data[--array->count]; 
}

// This doesn't free up the memory since we are already using the arena for allocations, therefore
// this will just reset the count back to 0.
template <typename T>
void array_reset(Array<T> *array) {
	assert(array);
	array->count = 0;
}

// Much like the Array<>, this String datatype will use a pre-existing arena to
// handle allocations and whatnot. 
struct String {
	char *data;
	u64   length;
};

String string_create(Arena *arena, const char *str);
String string_view(char *data, u64 length);
String string_concat(Arena *arena, String a, String b);
String string_slice(String str, u64 start, u64 end);
String string_format(Arena *arena, const char *fmt, ...);
bool   string_comp(String a, String b); // Should this return an integer instead?

const char *string_to_cstr(String str);

// The resulting string point to read-only memory, so don't write to it!
#define string_literal_create(s) String{(char *)(s), sizeof(s) - 1}

// This StringBuilder would break if we called arena_allocate() in between each append.
// Therefore, we must finish all appends with the StringBuider before doing any more arena
// allocations, or else the memory will not be contigious!
struct StringBuilder {
	String buffer;
};

void   strbuild_append_string(Arena *arena, StringBuilder *sb, String s);
void   strbuild_append_cstring(Arena *areana, StringBuilder *sb, const char *cstr);
void   strbuild_fmt(Arena *arena, StringBuilder *sb, const char *fmt, ...);
String strbuild_terminate(Arena *arena, StringBuilder *sb);
void   strbuild_reset(StringBuilder *sb);

// Raylib expects a null-terminated string here, so we can't pass a view into a string unless we
// manually put the null-terminator.
void draw_text_ex_with_string(const Font *font, String str, Vector2 pos, s32 font_size, s32 spacing, Color color);

#endif
