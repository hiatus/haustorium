#include <linux/ftrace.h>
#include <linux/module.h>
#include <linux/types.h>

#include "config.h"
#include "hook.h"
#include "htm.h"

#if HTM_KPROBE_LOOKUP
#include <linux/kprobes.h>

extern struct kprobe kp;

struct kprobe kp = {
	.symbol_name = "kallsyms_lookup_name"
};
#endif

typedef unsigned long (*_kallsyms_lookup_name_t)(const char *name);
static _kallsyms_lookup_name_t _kallsyms_lookup_name = NULL;

static notrace int _resolve_hook_address(struct htm_ftrace_hook *hook);
static notrace void _ftrace_thunk(unsigned long ip, unsigned long parent_ip,
                                  struct ftrace_ops *ops, struct ftrace_regs *ftregs);

static notrace int _resolve_hook_address(struct htm_ftrace_hook *hook)
{
	hook->address = htm_resolve_sym(hook->name);

	if (! hook->address)
		return -ENOENT;

	#if HTM_USE_FENTRY_OFFSET
	*((unsigned long *)hook->original) = hook->address + MCOUNT_INSN_SIZE;
	#else
	*((unsigned long *)hook->original) = hook->address;
	#endif

	return 0;
}

static notrace void _ftrace_thunk(unsigned long ip, unsigned long parent_ip,
                                  struct ftrace_ops *ops, struct ftrace_regs *ftregs)
{
	struct pt_regs *regs = ftrace_get_regs(ftregs);
	struct htm_ftrace_hook *hook = container_of(ops, struct htm_ftrace_hook, ops);

	#if HTM_USE_FENTRY_OFFSET
	regs->ip = (unsigned long)hook->function;
	#else
	if (! within_module(parent_ip, THIS_MODULE))
		regs->ip = (unsigned long)hook->function;
	#endif
}

notrace unsigned long htm_resolve_sym(const char *symname)
{
	unsigned long address;

	if (! _kallsyms_lookup_name) {
		#if HTM_KPROBE_LOOKUP
		register_kprobe(&kp);
		_kallsyms_lookup_name = (_kallsyms_lookup_name_t)kp.addr;
		unregister_kprobe(&kp);
		#else
		_kallsyms_lookup_name = &kallsyms_lookup_name;
		#endif

		#ifdef HTM_LOG
		htm_pr_debug("address of kallsyms_lookup_name: 0x%p", _kallsyms_lookup_name);
		#endif
	}

	address = _kallsyms_lookup_name(symname);

	#ifdef HTM_LOG
	if (address)
		htm_pr_debug("address of %s: 0x%016zx", symname, sym);
	#endif

	return address;
}

notrace int htm_hook_install(struct htm_ftrace_hook *hook)
{
	int ret;

	if (hook->enabled) {
		#ifdef HTM_DEBUG
		htm_pr_warn("hook already installed: %s", hook->name);
		#endif

		return -EINVAL;
	}

	if ((ret = _resolve_hook_address(hook))) {
		#ifdef HTM_DEBUG
		htm_pr_err("failed to resolve symbol: %s", hook->name);
		#endif

		return ret;
	}

	hook->ops.func  = (ftrace_func_t)_ftrace_thunk;
	hook->ops.flags = FTRACE_OPS_FL_SAVE_REGS | FTRACE_OPS_FL_RECURSION | FTRACE_OPS_FL_IPMODIFY;

	if ((ret = ftrace_set_filter_ip(&hook->ops, hook->address, 0, 0))) {
		#ifdef HTM_DEBUG
		htm_pr_err(
			"cannot hook symbol %s - ftrace_set_filter_ip() returned %d for address %p",
			hook->name, ret, (void *)hook->address
		);
		#endif

		return ret;
	}

	if ((ret = register_ftrace_function(&hook->ops))) {
		#ifdef HTM_DEBUG
		htm_pr_err(
			"cannot hook symbol %s - register_ftrace_function() returned %d",
			hook->name, ret
		);
		#endif

		return ret;
	}

	hook->enabled = 1;

	#ifdef HTM_DEBUG
	htm_pr_notice("installed hook for symbol %s", hook->name);
	#endif

	return ret;
}

notrace int htm_hook_uninstall(struct htm_ftrace_hook *hook)
{
	int ret;

	if (! hook->enabled) {
		#ifdef HTM_DEBUG
		htm_pr_warn("symbol not hooked: %s", hook->name);
		#endif

		return -EINVAL;
	}

	if ((ret = unregister_ftrace_function(&hook->ops))) {
		#ifdef HTM_DEBUG
		htm_pr_err(
			"cannot unhook symbol %s - unregister_ftrace_function() returned %d",
			hook->name, ret
		);
		#endif

		return ret;
	}

	if ((ret = ftrace_set_filter_ip(&hook->ops, hook->address, 1, 0))) {
		#ifdef HTM_DEBUG
		htm_pr_warn(
			"when unhooking symbol %s, ftrace_set_filter_ip() returned %d for address "
			"%p - hook still considered uninstalled as unregister_ftrace_function() "
			"returned 0", hook->name, ret, (void *)hook->address
		);
		#endif
	}

	hook->enabled = 0;

	#ifdef HTM_DEBUG
	htm_pr_notice("uninstalled hook for symbol %s", hook->name);
	#endif

	return ret;
}
