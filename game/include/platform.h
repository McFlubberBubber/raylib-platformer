#ifndef PLATFORM_H
#define PLATFORM_H

#include <stddef.h>
#include <stdint.h>

// This header file is the foundation for any platform-specific code that the
// project may need. For example, the arenas implemented within the general.h
// currently use Windows' VirtualAlloc() and POSIX's mmap().
//
// The source code for each platform layer will be named accordingly. For example,
// win32_platform.cpp, etc.

void *platform_allocate(size_t size);
void  platform_free(void *ptr, size_t size);

typedef uint64_t FileTimestamp;
FileTimestamp platform_get_file_timestamp(const char *path);

#endif
