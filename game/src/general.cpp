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

// This function ASSUMES that the string we pass into this procedure actually has a null-terminated
// string at the end of it. Most of the regular string procedures that are within this file do ensure
// we have a null-terminator at the end of the string, EXCEPT for the StringBuilder, as of
// 2/05/2026.
const char *string_to_cstr(String str) {
	return (const char *)str.data;
}

bool string_is_empty(String str) {
	return str.data == nullptr || str.length == 0;
}

bool string_starts_with(String str, String prefix) {
	if (prefix.length > str.length) return false;
	return memcmp(str.data, prefix.data, prefix.length) == 0;
}

bool string_ends_with(String str, String suffix) {
	if (suffix.length > str.length) return false;
	s32 length_difference = str.length - suffix.length;
	return memcmp(str.data + length_difference, suffix.data, suffix.length) == 0;
}

s64 string_find_char(String str, char c) {
	for (u64 i = 0; i < str.length; ++i) {
		if (str.data[i] == c) return (s64)i;
	}
	return -1;
}

s64 string_find_char_reverse(String str, char c) {
	for (u64 i = str.length; i > 0; --i) {
		if (str.data[i - 1] == c) return (s64)i;
	}
	return -1;
}

// Arena-allocating string manipulation.
String string_copy(Arena *arena, String str) {
	assert(arena);
	if (string_is_empty(str)) return {};

	char *data = (char *)arena_allocate(arena, (str.length + 1), alignof(char)); // +1 for null-term.
	assert(data);
	memcpy(data, str.data, str.length);
	data[str.length] = '\0';

	return {data, str.length};
}

String string_trim_left(String s) {
    while (s.length > 0 && (s.data[0] == ' ' || s.data[0] == '\t')) {
        s.data++;
        s.length--;
    }
    return s;
}

String string_trim_right(String s) {
    while (s.length > 0 && (s.data[s.length - 1] == ' ' || s.data[s.length - 1] == '\t')) {
        s.length--;
    }
    return s;
}

String string_trim(String s) {
    return string_trim_right(string_trim_left(s));
}

s32 string_split_whitespace(String str, String *out, s32 max_tokens) {
	assert(out);
	s32 count = 0;
	u64 i = 0;

	while (i < str.length && count < max_tokens) {
		while (i < str.length && str.data[i] == ' ') ++i;
		if (i >= str.length) break;

		// Finding the end of the token.
		u64 start = i;
		while (i < str.length && str.data[i] != ' ' ) ++i;
		out[count++] = string_view(str.data + start, i - start);
	}

	return count;
}


// ===== String Builder stuff... =====
static void strbuild_append(Arena *arena, StringBuilder *sb, const char *data, u64 length) {
	assert(arena && sb && data);
	char *dest = (char *)arena_allocate(arena, length, alignof(char));
	assert(dest);
	if (sb->buffer.data == nullptr) {
		sb->buffer.data = dest;
	}

	memcpy(dest, data, length);
	sb->buffer.length += length;
}

void strbuild_append_string(Arena *arena, StringBuilder *sb, String str) {
	strbuild_append(arena, sb, str.data, str.length);
}

void strbuild_append_cstring(Arena *arena, StringBuilder *sb, const char *data) {
	assert(data);
	u64 length = 0;
	while (data[length]) length++;
	strbuild_append(arena, sb, data, length);
}

void strbuild_fmt(Arena *arena, StringBuilder *sb, const char *fmt, ...) {
	assert(arena && sb && fmt);
	va_list args;

	va_start(args, fmt);
	int needed = vsnprintf(nullptr, 0, fmt, args);
	va_end(args);

	assert(needed > 0);
	char *dest = (char *)arena_allocate(arena, (u64)(needed + 1), alignof(char));
	assert(dest);

	va_start(args, fmt);
	vsnprintf(dest, (u64)(needed + 1), fmt, args);
	va_end(args);

	if (sb->buffer.data == nullptr) {
		sb->buffer.data = dest;
	}
	sb->buffer.length += (u64)needed;
}

// Since the StringBuilder does not put in the null-terminator like our String datatype already
// does, we need to ensure that we call this function to slap that terminator once we are done
// building our string. It kinda sucks, but if we do plan on eventually using the string to render
// stuff on raylib (which expects null-terminated strings), we need to do this...
String strbuild_terminate(Arena *arena, StringBuilder *sb) {
	char *null_term = (char *)arena_allocate(arena, 1, alignof(char));
	assert(null_term);
	*null_term = '\0';
	return sb->buffer;
}

void strbuild_reset(StringBuilder *sb) {
	sb->buffer.data   = nullptr;
	sb->buffer.length = 0;
}

void draw_text_ex_with_string(const Font *font, String str, Vector2 pos, s32 font_size, s32 spacing, Color color) {
	if (str.data) {
		DrawTextEx(*font, string_to_cstr(str), pos, font_size, spacing, color);
	} else {
		DrawTextEx(*font, "ERROR!", pos, font_size, spacing, RED);
		fprintf(stderr, "[DRAW_TEXT_WITH_STRING] Received null String at pos (%.1f, %.1f)\n", pos.x, pos.y);
	}
}

Vector2 measure_text_ex_with_string(const Font *font, String str, float font_size, float font_spacing) {
	if (str.data) {
		return MeasureTextEx(*font, string_to_cstr(str), font_size, font_spacing);
	} else {
		fprintf(stderr, "[MEASURE_TEXT_WITH_STRING] Received a String with NULL data.\n");
		return { 0, 0 };
	}
}
