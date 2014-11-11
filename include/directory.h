int mkdir2 (char *pathname);
int rmdir2 (char *pathname);

DIR2 opendir2 (char *pathname);
int readdir2 (DIR2 handle, DIRENT2 *dentry);
int closedir2 (DIR2 handle);

int chdir2 (char *pathname);
int getcwd2 (char *pathname, int size);
