#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/syscall.h>
#include <stdio.h>

#include "load.h"
#include "config.h"

extern const unsigned char _mod_start[];
extern const unsigned char _mod_end[];

#define MOD_SIZE ((size_t)(_mod_end - _mod_start))

int htm_load(void)
{
	int ret = 0;

	if (syscall(__NR_init_module, _mod_start, MOD_SIZE, ""))
		ret = -1;

	return ret;
}

int htm_unload(void)
{
	return syscall(__NR_delete_module, HTM_MODULE_NAME, O_NONBLOCK) ? -1 : 0;
}
