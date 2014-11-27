#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/t2fs.h"
#include "../include/file.h"
#include "../include/utility.h"
#include "../include/apidisk.h"

int y = 0;

t2fs_file handlers[20]; //guarda registro dos arquivos abertos com create/open
unsigned int next_handle = 0; //variavel de controle para saber em qual handle estah a execucao
unsigned int counter_opened_files = 0; //armazena descritores abertos
file fileDescriptors[20]; //guarda registro dos arquivos abertos com create/open
int usedDescriptors[20]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

int bitmapSector = 1;

t2fs_file create2 (char *filename){
	GetDiskInformation();

    if(counter_opened_files >= 20){
        printf("Ja existem 20 arquivos abertos!\n");
        exit(1);
    }
    int empty;

    char *name;
    name = ExtendName(filename);//precisa colocar so o nome e nao o path inteiro
    getfileName(name);

    t2fs_record* new_record = (t2fs_record*)malloc(sizeof(t2fs_record));
    if(strstr(name, ".") != NULL) new_record->TypeVal = 1;
    else new_record->TypeVal = 2;
    
    memcpy(new_record->name, name, 39);//sizeof(nome));
    new_record->name[39] = 0;
    new_record->blocksFileSize =0;
    new_record->bytesFileSize = 0;
    new_record->dataPtr[0] = 0;
    new_record->dataPtr[1] = 0;
    new_record->singleIndPtr = 0;
    new_record->doubleIndPtr = 0;	
	    
	int freePos = readRecordsDir(2);
	
	if(freePos != 99)
		InsertRecord(2,  freePos*64, new_record);
	
	empty = searchEmptyDescriptor();
    strcpy(fileDescriptors[empty].name, name);
    fileDescriptors[empty].currentPointer=0;
    usedDescriptors[empty] = 1;
    
    counter_opened_files += 1;
    printf("\nARQUIVO %s CRIADO COM SUCESSO\n", name);

    return empty;
}

int delete2 (char *filename){
	return 1;
}

t2fs_file open2 (char *filename){
	GetDiskInformation();

	if(counter_opened_files >= 20){
		printf("\n Voce ja possui 20 arquivos abertos. \n");
		return -1;
	}
	else{
		int empty;
		empty = searchEmptyDescriptor();
		strcpy(fileDescriptors[empty].name,filename); //descriptor recebe as informacoes do arquivo aberto
		fileDescriptors[empty].currentPointer = 0;
		usedDescriptors[empty] = 1; 

		return empty;
	}
}

int close2 (t2fs_file handle){
	usedDescriptors[handle] = 0;
	counter_opened_files--;
	
	return 1;
}
int read2 (t2fs_file handle, char *buffer, int size){
	if(usedDescriptors[handle]==0) //handle invalido
		return -1;
	t2fs_record fileRec = getRecord(fileDescriptors[handle].name, superbloco.RootDirReg);
	int currentBlock, readBytes, bytesLeft, bufferPointer=0, retorno, firstBlock;
	if(fileRec.blocksFileSize > 0){
		if(size + fileDescriptors[handle].currentPointer > fileRec.bytesFileSize){
			retorno = 0;
			size = fileRec.bytesFileSize - fileDescriptors[handle].currentPointer;
		}
		bytesLeft = size;
		currentBlock = 1+ (fileDescriptors[handle].currentPointer/1024); //seleciona a posicao do bloco que contem o byte apontado por currentPointer
		firstBlock = currentBlock;		
		while(bytesLeft>0){
			read_block(getNextReadableBlock(fileRec, currentBlock), genericBlock);
			if(currentBlock == firstBlock)			
				readBytes = fileDescriptors[handle].currentPointer%1024;
			else
				readBytes = 0;			
			while(readBytes < superbloco.BlockSize && readBytes <= bytesLeft){
					memcpy(buffer+bufferPointer, genericBlock+readBytes, sizeof(char));//copia um byte apontado por readBytes do bloco do disco para o buffer na posicao bufferPointer
					readBytes++;
					bufferPointer++;
					bytesLeft--;
					fileDescriptors[handle].currentPointer++;
			}
			currentBlock++;
		}
		if(retorno != 0)
			retorno = size;
		return retorno;
	}
	return -1;
}
int write2 (t2fs_file handle, char *buffer, int size){
	if(usedDescriptors[handle]==0) //handle invalido
		return -1;
	t2fs_record fileRec = getRecord(fileDescriptors[handle].name, superbloco.RootDirReg);
	int currentBlock , wroteBytes, bytesLeft, bufferPointer=0, firstBlock, nextBlock;
	bytesLeft = size;
	currentBlock = 1+ (fileDescriptors[handle].currentPointer/1024);
	firstBlock = currentBlock;
	while(bytesLeft>0){
		nextBlock = getNextReadableBlock(fileRec, currentBlock);
		if(nextBlock!=0xFFFFFFFF)
			read_block(nextBlock, genericBlock);
		else
			read_block(searchWritableBlock(&fileRec), genericBlock);
		if(currentBlock == firstBlock)
			wroteBytes =fileDescriptors[handle].currentPointer%1024;
		else
			wroteBytes = 0;
		while(wroteBytes < superbloco.BlockSize && wroteBytes <= bytesLeft){
				memcpy(genericBlock+wroteBytes, buffer+bufferPointer, sizeof(char));
				wroteBytes++;
				bufferPointer++;
				bytesLeft--;
				fileDescriptors[handle].currentPointer++;
				if(fileDescriptors[handle].currentPointer == fileRec.bytesFileSize +2)
					fileRec.bytesFileSize++;
		}
		currentBlock++;
	}
	t2fs_record parentDir = getParentDirectory(fileDescriptors[handle].name);
	updateRecord(parentDir, fileRec, getDirectoryPositionByName(parentDir, fileRec.name));
	return size;
}
int seek2 (t2fs_file handle, unsigned int offset){
	return 1;
}
