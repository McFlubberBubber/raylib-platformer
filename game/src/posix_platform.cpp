#include "platform.h"

#include <sys/mman.h>
#include <assert.h>

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
