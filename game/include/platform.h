#ifndef PLATFORM_H
#define PLATFORM_H

#include <stddef.h>

// This header file is the foundation for any platform-specific code that the
// project may need. For example, the arenas implemented within the general.h
// currently use Windows' VirtualAlloc().
//
// The source code for each platform layer will be named accordingly. For example,
// win32_platform.cpp, etc.

void *platform_allocate(size_t size);
void  platform_free(void *ptr, size_t size);

#endif
