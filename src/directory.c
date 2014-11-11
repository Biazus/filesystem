#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/t2fs.h"
#include "../include/directory.h"
#include "../include/apidisk.h"

int mkdir2 (char *pathname){
	return 1;
}

int rmdir2 (char *pathname){
	return 1;
}

//DIR2 opendir2 (char *pathname){
	
//}

int readdir2 (DIR2 handle, DIRENT2 *dentry){
	return 1;
}

int closedir2 (DIR2 handle){
	return 1;
}

int chdir2 (char *pathname){
	return 1;
}

int getcwd2 (char *pathname, int size){
	return 1;
}
