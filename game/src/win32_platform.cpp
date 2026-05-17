// Defines to reduce the headache of the windows header file...
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#define NOUSER

#include <windows.h>

#include "platform.h"

void *platform_allocate(size_t size) {
	void *result = VirtualAlloc(nullptr, size, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
	return result; // Will return nullptr on failure.
}

void platform_free(void *ptr, size_t size) {
	if (!ptr) return;
	
	(void)size;
	VirtualFree(ptr, 0, MEM_RELEASE);
}
