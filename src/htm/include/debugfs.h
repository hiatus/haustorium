#ifndef HTM_DEBUGFS_H
#define HTM_DEBUGFS_H

int htm_dfs_create_all(void);
int htm_dfs_create_rsh(void);
int htm_dfs_remove_all(void);
int htm_dfs_remove_rsh(void);

char *htm_dfs_name_dir(void);
char *htm_dfs_name_rsh(void);
char *htm_dfs_name_bashrc(void);
#endif
