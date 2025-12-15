#ifndef HTM_DEBUGFS_H
#define HTM_DEBUGFS_H

int htm_debugfs_create_all(void);
int htm_debugfs_create_rsh(void);
int htm_debugfs_remove_all(void);
int htm_debugfs_remove_rsh(void);

char *htm_debugfs_name_dir(void);
char *htm_debugfs_name_rsh(void);
char *htm_debugfs_name_bashrc(void);
#endif
