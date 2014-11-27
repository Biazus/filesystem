teste: libt2fs.a
	gcc -o ../bin/teste teste.c -L../lib -lt2fs -lt2fs -Wall
 
libt2fs.a: t2fs.o 
	ar crs ../lib/libt2fs.a file.o directory.o utility.o ../lib/apidisk.o
 
t2fs.o: ../src/file.c ../src/directory.c ../src/utility.c
	rm -rf *.o	
	gcc -c ../src/file.c -Wall
	gcc -c ../src/directory.c -Wall
	gcc -c ../src/utility.c -Wall
 
clear:
	rm -rf *.o
	rm ../lib/libt2fs.a
