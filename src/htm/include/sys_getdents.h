#ifndef HTM_SYS_GETDENTS_H
#define HTM_SYS_GETDENTS_H

#include "hook.h"

#ifndef HTM_LINUX_DIRENT_DEFINED
#define HTM_LINUX_DIRENT_DEFINED

struct linux_dirent {
    unsigned long d_ino;
    unsigned long d_off;
    unsigned short d_reclen;
    char d_name[];
};

struct linux_dirent64 {
    u64 d_ino;
    s64 d_off;
    unsigned short d_reclen;
    unsigned char d_type;
    char d_name[];
};
#endif

extern struct htm_ftrace_hook hook_sys_getdents;
extern struct htm_ftrace_hook hook_sys_getdents64;
#endif
