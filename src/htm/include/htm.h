#ifndef HTM_H
#define HTM_H

#ifndef __KERNEL_PRINTK__
#include <linux/printk.h>
#endif


#define htm_pr_debug(s, ...) \
	pr_debug(KBUILD_MODNAME ":%s: " s "\n", __FUNCTION__, ##__VA_ARGS__)

#define htm_pr_info(s, ...) \
	pr_info(KBUILD_MODNAME ":%s: " s "\n", __FUNCTION__, ##__VA_ARGS__)

#define htm_pr_notice(s, ...) \
	pr_notice(KBUILD_MODNAME ":%s: " s "\n", __FUNCTION__, ##__VA_ARGS__)

#define htm_pr_warn(s, ...) \
	pr_warn(KBUILD_MODNAME ":%s: " s "\n", __FUNCTION__, ##__VA_ARGS__)

#define htm_pr_err(s, ...) \
	pr_err(KBUILD_MODNAME ":%s: " s "\n", __FUNCTION__, ##__VA_ARGS__)

#define htm_pr_crit(s, ...) \
	pr_crit(KBUILD_MODNAME ":%s: " s "\n", __FUNCTION__, ##__VA_ARGS__)

#define htm_pr_alert(s, ...) \
	pr_alert(KBUILD_MODNAME ":%s: " s "\n", __FUNCTION__, ##__VA_ARGS__)

#define htm_pr_emerg(s, ...) \
	pr_emerg(KBUILD_MODNAME ":%s: " s "\n", __FUNCTION__, ##__VA_ARGS__)
#endif
