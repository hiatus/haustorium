#include <linux/sched.h>
#include <linux/slab.h>
#include <linux/version.h>

#if LINUX_VERSION_CODE < KERNEL_VERSION(4, 14, 0)
#include <linux/kmod.h>
#else
#include <linux/umh.h>
#endif

#include "config.h"
#include "exec.h"
#include "htm.h"

static char _argv2[HTM_MAX_STRING];
static char *_argv[] = {HTM_EXEC_SHELL, "-c", _argv2, NULL};
static char *_envp[] = HTM_EXEC_ENVP;

notrace int htm_exec(const char *cmd, int wait)
{
	int ret;

	_argv2[strlen(cmd)] = 0x00;
	strncpy(_argv2, cmd, HTM_MAX_STRING);

	#ifdef HTM_DEBUG
	htm_pr_info("executing: %s", _argv2);
	#endif

	ret = call_usermodehelper(_argv[0], _argv, _envp, wait);

	#ifdef HTM_DEBUG
	if (ret)
		htm_pr_err("call_usermodehelper failed with code %i", ret);
	#endif

	return ret;
}

notrace int htm_exec_argv(char **argv, int wait)
{
	int ret;

	#ifdef HTM_DEBUG
	htm_pr_info("executing binary %s", argv[0]);

	for (size_t i = 0; argv[i]; ++i)
		htm_pr_info("argv[%zu]: %s", i, argv[i]);
	#endif

	ret = call_usermodehelper(argv[0], argv, _envp, wait);

	#ifdef HTM_DEBUG
	if (ret)
		htm_pr_err("call_usermodehelper failed with code %i", ret);
	#endif

	return ret;
}
