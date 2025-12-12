#ifndef HTM_COMMAND_H
#define HTM_COMMAND_H

#include <linux/types.h>

bool is_htm_command(const char *cmd);
bool is_htm_command_atomic(const char *cmd);
int htm_command(const char *cmd, __be32 *saddr);
#endif
