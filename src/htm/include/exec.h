#ifndef HTM_EXEC_H
#define HTM_EXEC_H

int htm_exec(const char *cmd, int wait);
int htm_exec_argv(char **argv, int wait);
#endif
