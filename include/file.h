t2fs_file create2 (char *filename);
int delete2 (char *filename);
t2fs_file open2 (char *filename);
int close2 (t2fs_file handle);
int read2 (t2fs_file handle, char *buffer, int size);
int write2 (t2fs_file handle, char *buffer, int size);
int seek2 (t2fs_file handle, unsigned int offset);
