all:
	rm -rf *.o
	rm ./lib/libt2fs.a
	gcc -c ./src/file.c -Wall
	gcc -c ./src/directory.c -Wall
	gcc -c ./src/utility.c -Wall
	ar crs ./lib/libt2fs.a file.o directory.o utility.o ./lib/apidisk.o
	
libt2fs.a: t2fs.o 
	ar crs ./lib/libt2fs.a file.o directory.o utility.o ./lib/apidisk.o
	
t2fs.o: ../src/file.c ../src/directory.c ../src/utility.c
	rm -rf *.o
	rm ../lib/libt2fs.a
	gcc -c ./src/file.c -Wall
	gcc -c ./src/directory.c -Wall
	gcc -c ./src/utility.c -Wall

clear:
	rm -rf *.o
	rm ./lib/libt2fs.a
