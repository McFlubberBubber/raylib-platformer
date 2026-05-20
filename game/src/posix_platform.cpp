#include "platform.h"

#include <assert.h>

#include <sys/mman.h>
#include <sys/stat.h>
#include <time.h>

// @TODO: Test if literally any of these procedures work cuz wtf.
void *platform_allocate(size_t size) {
	void *result = mmap(nullptr, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

	if (result == MAP_FAILED) return nullptr;
	return result;
}

void platform_free(void *ptr, size_t size) {
	if (!ptr) return;

	int result = munmap(ptr, size);
	assert(result == 0);
}

FileTimestamp platform_get_file_timestamp(const char *path) {
	FileTimestamp result;
	struct stat file_stat;

	if (stat(path, &file_stat) == 0) {
		result = file_stat.st_mtime;
	} else {
		result = 0;
	}

	return result;
}
