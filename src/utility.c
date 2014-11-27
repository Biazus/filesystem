#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/t2fs.h"
#include "../include/utility.h"
#include "../include/apidisk.h"

int geometryLoaded = 0;

WORD    Version;        /* Versão atual desse sistema de arquivos: (valor fixo 7DE=2014; 2=2 semestre). */
WORD    SuperBlockSize; /* Quantidade de setores  que formam o superbloco. (fixo em 1 setor) */
DWORD   DiskSize;       /* Tamanho total da partição T2FS, incluindo o tamanho do superblock. */
DWORD   NofBlocks;      /* Quantidade total de blocos de dados na partição T2FS (1024 blocos). */
DWORD   BlockSize;      /* Tamanho de um bloco.*/
char    Reserved[108];  /* Não usados */
t2fs_record BitMapReg;  /* Registro que descreve o arquivo que mantém o bitmap de blocos livres e ocupados */
t2fs_record RootDirReg; /* Registro que descreve o arquivo que mantém as entradas do diretório raiz */
file fileDescriptors[20]; //guarda registro dos arquivos abertos com create/open
int usedDescriptors[20]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

char ctrlSize = 1;
int diskSize = 256;
short int blockSize = 256;
short int freeBlockSize = 1;
short int rootSize = 16;
short int fileEntry = 64;

int identify2 (char *name, int size){
	name = "Pedro Morales 143049 Miller Biazus 187984\0";
	return 1;
}

char* ExtendName(char *nome)
{
    char* extName = (char *)malloc(39);
    int i = 0;

    while (nome[i] != 0){
		extName[i] = nome[i];
		i++;
	}
	while (i<39){
		extName[i] = 0;
		i++;
	}
	return (extName);
}

int read_block (unsigned int block, char *buffer){
       unsigned int aux_sector=block*4;
       char aux_block[256];
       if(read_sector(aux_sector, aux_block)!= 0) return -1;
       memcpy(buffer, aux_block, 256 * sizeof(char));
       if(read_sector(aux_sector+1, aux_block)!= 0) return -1;
       memcpy(buffer + 256, aux_block, 256 * sizeof(char));
       if(read_sector(aux_sector+2, aux_block)!= 0) return -1;
       memcpy(buffer + 512, aux_block, 256 * sizeof(char));
       if(read_sector(aux_sector+3, aux_block)!= 0) return -1;
       memcpy(buffer + 768,  aux_block, 256 * sizeof(char));
       return 0;
}

//transforma funcao de write_sector em write_block
int write_block (unsigned int block, char *buffer){
       unsigned int aux_sector=block*4;
       char aux_block[256];
       memcpy(aux_block, buffer, 256 * sizeof(char));
       if(write_sector(aux_sector, aux_block)!= 0) return -1;
       memcpy(aux_block, buffer+256, 256 * sizeof(char));
       if(write_sector(aux_sector+1, aux_block)!= 0) return -1;
       memcpy(aux_block, buffer + 512, 256 * sizeof(char));
       if(write_sector(aux_sector+2, aux_block)!= 0) return -1;
       memcpy(aux_block, buffer + 768, 256 * sizeof(char));
       if(write_sector(aux_sector+3, aux_block)!= 0) return -1;
       return 0;
}

void GetDiskInformation()
{
	int read_return = read_block(0,genericBlock);
	if(read_return==-1);
	if(genericBlock[0] == 'T' && genericBlock[1] == '2' && genericBlock[2] == 'F' && genericBlock[3] == 'S')
	{
		
		superbloco.Id[0] = genericBlock[0];
		superbloco.Id[1] = genericBlock[1];
		superbloco.Id[2] = genericBlock[2];
		superbloco.Id[3] = genericBlock[3];
		superbloco.Version = *((WORD *)(genericBlock + 4));
		superbloco.SuperBlockSize = *((WORD *)(genericBlock + 6));
		superbloco.DiskSize = *((DWORD *)(genericBlock + 8));
		superbloco.NofBlocks = *((DWORD *)(genericBlock + 12)); //antigo diskSize
		superbloco.BlockSize = *((DWORD *)(genericBlock + 16)); //antigo blockSize
		
		//printf("\n Version: %i SuperBlockSize: %d Disksize: %d NofBlocks: %d BlockSize: %d \n",superbloco.Version,
		//																				  superbloco.SuperBlockSize,
		//																				  superbloco.DiskSize,
		//																				  superbloco.NofBlocks,
		//																				  superbloco.BlockSize);
	}
	else
	{
		int i=0;
		for (i=0; i < sizeof(genericBlock); i++)
			printf("%c",genericBlock[i]);
		//printf("\n Nao eh um disco  T2FS! \n");
		DiskInit();
		GetDiskInformation();
		exit(1);
	}
}

void DiskInit()
{
	//initBitmap();
	//initRoot();
	//printf("\n Criando um T2FS...! \n");
    int i=0;
    for(i=0;i<superbloco.NofBlocks;i++)
        genericBlock[i]=0;
    genericBlock[0]=(BYTE)'\x54';
    genericBlock[1]=(BYTE)'\x32';
    genericBlock[2]=(BYTE)'\x46';
    genericBlock[3]=(BYTE)'\x53';
    genericBlock[4]=(BYTE)'\xE2';
    genericBlock[5]=(BYTE)'\x7D';
    genericBlock[6]=(BYTE)'\x01';
    genericBlock[7]=(BYTE)'\x00';
    genericBlock[8]=(BYTE)'\x00';
    genericBlock[9]=(BYTE)'\x01';
    genericBlock[10]=(BYTE)'\x10';
    genericBlock[11]=(BYTE)'\x00';
    genericBlock[12]=(BYTE)'\x00';
    genericBlock[13]=(BYTE)'\x04';
    genericBlock[14]=(BYTE)'\x00';
    genericBlock[15]=(BYTE)'\x00';
    genericBlock[16]=(BYTE)'\x00';
    genericBlock[17]=(BYTE)'\x04';
    genericBlock[18]=(BYTE)'\x00';
    genericBlock[19]=(BYTE)'\x00';
    
    //tipo invalido
    genericBlock[128]=(BYTE)'\xFF';
    //nome: BITMAP
    genericBlock[129]=(BYTE)'\x42';//B
    genericBlock[130]=(BYTE)'\x49';
    genericBlock[131]=(BYTE)'\x54';
    genericBlock[132]=(BYTE)'\x4d';
    genericBlock[133]=(BYTE)'\x41';
    genericBlock[134]=(BYTE)'\x50';
    genericBlock[135]=(BYTE)'\x00';
    
    //tipo diretorio
    genericBlock[192]=(BYTE)'\xFF';
    //nome: ROOT
    genericBlock[193]=(BYTE)'\x2F';
    genericBlock[194]=(BYTE)'\x52';
    genericBlock[195]=(BYTE)'\x4F';
    genericBlock[196]=(BYTE)'\x4F';
    genericBlock[197]=(BYTE)'\x54';
    genericBlock[198]=(BYTE)'\x00';
    write_sector(0, genericBlock);
}

void createBitmap(t2fs_record* newArq){
	newArq->TypeVal = 0xFF;
	strcpy(newArq->name,"Bitmap");
	newArq->blocksFileSize = 1;
	newArq->bytesFileSize = 128;
	newArq->dataPtr[0] = 0;
	newArq->dataPtr[1] = 0xFFFFFFFF;
	newArq->singleIndPtr = 0xFFFFFFFF;
	newArq->doubleIndPtr = 0xFFFFFFFF;
}

//inicializa o vetor do bitmap com zeros
void initBitmap(){
	int i;
	for(i=0;i<128;i++)
		bitmap[i]=0;
	write_block(1, (char*)bitmap);
}

DWORD searchEmptyBlock(){
	int dataBlockInit = 2; //0->bitmap, 1->root
	int bitLinha, bitColuna;
	DWORD i;
	BYTE auxBlock[128];
	
	read_block(dataBlockInit, genericBlock);	
	for(i=0; i < 128; i++)
      memcpy(&auxBlock[i], &genericBlock[i*sizeof(BYTE)], sizeof(BYTE));
	 
	for(i=dataBlockInit; i < 1024; i++){//numero de blocos
		bitLinha = i/8;
		bitColuna = i%8;
		BYTE aux = 0b10000000 >> bitColuna;
		if(!(auxBlock[bitLinha] & aux))
			return i;
	}
	return 0xFFFFFFFF;
}

void initRoot(){ //inicializa os records do diretorio root
	int i;
	for(i=0; i<16; i++){
		root[i].TypeVal= 0xFF;
		root[i].name[0] = '\0';
		root[i].blocksFileSize = 0;
		root[i].bytesFileSize = 0;
		root[i].dataPtr[0] = 0xFFFFFFFF;
		root[i].dataPtr[1] = 0xFFFFFFFF;
		root[i].singleIndPtr = 0xFFFFFFFF;
		root[i].doubleIndPtr = 0xFFFFFFFF;
	}
	for(i=0; i < 16; i++)
      memcpy(&genericBlock[i*sizeof(t2fs_record)], &root[i], sizeof(t2fs_record));
	
	write_block(2, genericBlock);
}

int searchEmptyRecord(t2fs_record* rec, DWORD sector){
	t2fs_record auxDir[16];
	int i;
	read_block(sector, genericBlock);
	for(i=0; i < 16; i++)
      memcpy(&auxDir[i], &genericBlock[i*sizeof(t2fs_record)], sizeof(t2fs_record));
	
	for(i=0; i<16; i++){
		if(auxDir[i].TypeVal == 0xFF){
			memcpy(rec, &auxDir[i], sizeof(t2fs_record));
			return 0;
		}
	}
	return -1;
}

//aloca no bloco -blockNumber- o registro -newrecord- a partir da posicao dada por -offset-
int InsertRecord(int blockNumber, int offset, t2fs_record* newrecord){	
    
	int j=0;
	//char bbloco[1024]; //bloco de recovery
	char bloco[superbloco.NofBlocks];
	int size= offset;
	if(read_block(blockNumber, bloco) != 0) //le o setor passado como param para backup, caso dê algo errado
		return -1;
	//strncpy(bloco, bbloco, sizeof(bbloco)); // copia  obloco para um novo bloco que pode ser alterado
	
	//abaixo o record eh colocado no disco de acordo com offsets especificados pelo professor.
	memcpy(bloco+size, &newrecord->TypeVal, sizeof(newrecord->TypeVal));
	size += sizeof(newrecord->TypeVal);
	memcpy(bloco+size, &newrecord->name, sizeof(newrecord->name));
	size += sizeof(newrecord->name);
	memcpy(bloco+size, &newrecord->blocksFileSize, sizeof(newrecord->blocksFileSize));
	size += sizeof(newrecord->blocksFileSize);
	memcpy(bloco+size, &newrecord->bytesFileSize, sizeof(newrecord->bytesFileSize));
	size += sizeof(newrecord->bytesFileSize);
	memcpy(bloco + size, &newrecord->dataPtr[0], sizeof(newrecord->dataPtr[0]));
	size += sizeof(newrecord->dataPtr[0]);
	memcpy(bloco+size, &newrecord->dataPtr[1], sizeof(newrecord->dataPtr[1]));
	size += sizeof(newrecord->dataPtr[1]);
	memcpy(bloco+size, &newrecord->singleIndPtr, sizeof(newrecord->singleIndPtr));
	size += sizeof(newrecord->singleIndPtr);
	memcpy(bloco+size, &newrecord->doubleIndPtr, sizeof(newrecord->doubleIndPtr));
	
	//escreve o registro no disco
	write_block(blockNumber, bloco);
	read_block(blockNumber, bloco);
	while(j < 1024){
		printf("%i", bloco[j]);
		j++;
	}
	printf("\n");
	return 0;
}

void initDataBlock(){
	int i;
	for(i=0; i<1024; i++)
		genericBlock[i] = 0;
}

//substitui uma posicao especifica do bitmap, de ocupado para livre e viceversa
void swapBitBitmap(DWORD posicao){
	int bitLinha, bitColuna;
	bitLinha = posicao/8;
	bitColuna = posicao % 8;
	BYTE aux = 0b10000000 >> bitColuna;
	bitmap[bitLinha] = bitmap[bitLinha] ^ aux;
}

DWORD createEmptyBlock(){ //cria um novo bloco de dados no disco, retorna o ponteiro para o setor alocado
	DWORD newBlock;
	newBlock = searchEmptyBlock(); //procura novo bloco vazio
	if(newBlock==-1){
		printf("Disk search ERROR: not enough space\n");
		return 0xFFFFFFFF;				
	}
	else{
		swapBitBitmap(newBlock);
		initDataBlock();
		write_block(newBlock, genericBlock);
		return newBlock;
	}
}

DWORD insertOnIndexBlock(DWORD indexSector){ 
	DWORD indexBlock[256], i;
	read_block(indexSector, genericBlock);
	for(i=0; i < 256; i++)
      memcpy(&indexBlock[i], &genericBlock[i*sizeof(DWORD)], sizeof(DWORD));
	for(i=0; i<256; i++)
		if(indexBlock[i] == 0xFFFFFFFF){
			indexBlock[i] = createEmptyBlock();
			for(i=0; i < 256; i++)
				memcpy(&genericBlock[i*sizeof(DWORD)], &indexBlock[i],  sizeof(DWORD));
			write_block(indexSector, genericBlock);
			return indexBlock[i];
		}
	return 0xFFFFFFFF;
}

DWORD searchLastIndexOnBlock(DWORD indexSector){
	DWORD indexBlock[256], i;
	read_block(indexSector, genericBlock);
	for(i=0; i < 256; i++)
      memcpy(&indexBlock[i], &genericBlock[i*sizeof(DWORD)], sizeof(DWORD));
	for(i=0; i<256; i++){
		if(indexBlock[i] == 0xFFFFFFFF)
			return indexBlock[i-1];
	}
	return 0xFFFFFFFF;
}

DWORD searchNextIndexOnBlock(DWORD indexSector, DWORD indexNumber){
	DWORD indexBlock[256], i;
	read_block(indexSector, genericBlock);
	for(i=0; i < 256; i++)
      memcpy(&indexBlock[i], &genericBlock[i*sizeof(DWORD)], sizeof(DWORD));
	return indexBlock[i-1];
}

void initIndexBlock(){
	int i;
	for(i=0; i<256; i++)
		indexBlock[i] = 0xFFFFFFFF;
}

DWORD createIndexBlock(){ //cria um novo bloco de indice no disco, retorna o ponteiro para o setor alocado
	int i;
	DWORD newIndexBlock;
	newIndexBlock = searchEmptyBlock(); //procura novo bloco vazio
	if(newIndexBlock==-1){
		printf("Disk search ERROR: not enough space\n");
		return 0xFFFFFFFF;				
	}
	else{
		swapBitBitmap(newIndexBlock);
		initIndexBlock();
		for(i=0; i < 256; i++)
			memcpy(&genericBlock[i*sizeof(DWORD)], &indexBlock[i], sizeof(DWORD));
		write_block(newIndexBlock, genericBlock);
		return newIndexBlock;
	}
}

DWORD searchWritableBlock(t2fs_record* rec){//A funcao chamadora se responsabiliza por atualizar record no diretorio pai
	DWORD index = 0xFFFFFFFF;
	DWORD sizeBytes = rec->bytesFileSize, sizeBlocks = rec->blocksFileSize;
	if(sizeBlocks == 0){ //ainda nao foi alocado nenhum bloco para o arquivo(logo apos o create)
		rec->dataPtr[0] = createEmptyBlock();
		rec->blocksFileSize++;
		index = rec->dataPtr[0];
	}
	if(sizeBlocks == 1){//arquivo so ocupa um bloco
		if(sizeBytes<superbloco.BlockSize)//e ele nao esta totalmente ocupado
			index = rec->dataPtr[0]; // o bloco que deve ser usado para escrita eh apontado pelo ponteiro direto
		else{ //o primeiro bloco ja esta cheio precisa alocar um segundo bloco
			rec->dataPtr[1] = createEmptyBlock();
			rec->blocksFileSize++;
			index = rec->dataPtr[1];
		}
	}
	if(sizeBlocks > 2 && sizeBlocks <= 258){
			if(rec->singleIndPtr == 0xFFFFFFFF)//Nao existe bloco apontado pelo ponteiro indireto
				rec->singleIndPtr = createIndexBlock(); //cria um novo bloco de indice
			if(sizeBytes%superbloco.BlockSize == 0){ //todos os blocos ja estao cheios
				index = insertOnIndexBlock(rec->singleIndPtr); //retorna um bloco apontado por um novo indice
				rec->blocksFileSize++;
			}
			else
				index = searchLastIndexOnBlock(rec->singleIndPtr); //retorna um bloco que ainda nao foi completamente usado apontado pelo ultimo indice do bloco de indice
		
	}
	else //ponteiros com dupla indirecao <<<<FALTA FAZER>>>>
		index = 0xFFFFFFFF;
	return index;  
}

DWORD getNextReadableBlock(t2fs_record rec, DWORD blockNumber){
	DWORD index = 0xFFFFFFFF;
	DWORD sizeBlocks = rec.blocksFileSize;
	if(blockNumber <= sizeBlocks){//nao ultrapassou o ultimo bloco do arquivo
		if(blockNumber == 1)
			index = rec.dataPtr[0];
		if(blockNumber == 2)
				index = rec.dataPtr[1];
		if(blockNumber > 2 && blockNumber <= 258)
				index = searchNextIndexOnBlock(rec.singleIndPtr, blockNumber-2);
		else //ponteiros com dupla indirecao <<<<FALTA FAZER>>>>
			index = 0xFFFFFFFF;
	}
	return index;  
}

//essa funcao recebe um path
//e retorna o dir mais a esquerda do path
char *getFirstPathDir(char *path){
	char strAux[15] = "/";
	char *token;
	token = strtok(path, "/");

    return strcat(strAux,token);
}

//essa funcao recebe um path
//e retorna outro path com o dir mais a esquerda removido
char *deleteFirstPathDir(char *path){ 
    
    char *pch;
    pch = strchr(path,'/');
    pch = strchr(pch+1,'/');
    if(pch != NULL){
		int position = pch-path+1;
		int len = strlen(path);
		memmove(path, path+position-1, len);
		return path;
    }
    else return path;
}

int getFreeDirectoryPosition(t2fs_record directoryRec){

      int currentBlock = 1;// checa-se se existe o bloco currentBlock no registro do diretorio
      int allocateNew = 0;// se achou algum lugar no bloco para escrever
      int freeSpace = 0;// se existe local para escrita no bloco
      int i = 0; //iterator
      char blocoLido[1024];
      while(!allocateNew && !freeSpace){

            int blockToCheck = 0;// bloco que deve-se checar por existencia de espaco livre para alocar record
            
            blockToCheck = getNextReadableBlock(directoryRec, currentBlock);
            if (blockToCheck == 0xFFFFFFFF){
                allocateNew = 1; //deve-se alocar mais blocos
            }
            else{      
            //bloco precisa ser checado para existencia de lugar livre  
                read_block(blockToCheck, blocoLido);
                t2fs_record blockOfRecords[16];
                for(i=0; i < 16; i++){
                    memcpy(&blockOfRecords[i], &blocoLido[i*sizeof(t2fs_record)], sizeof(t2fs_record)); //aloca bloco em um vetor de records
                    if(blockOfRecords[i].TypeVal == 0xFF){ //existe posicao vazia no diretorio atual
                        freeSpace = 1;
                        return ((1024 * (currentBlock-1))+i); //retorna o offset para o local que pode-se escrever
                    }
                }
            }
            currentBlock++;
      }
      if(!freeSpace){ //alocar novo bloco para escrever o registro
            //searchWritableBlock(directoryRec);
               
            return (1024*currentBlock);    
      }
      return 0;
}

int searchEmptyDescriptor(){
		int i=0;
		while(usedDescriptors[i]==1 && i<20)
			i++;
		return i;
}


//retorna o prieiro local do dir onde pode-se escrever
//os printfs dentro da funcao serviriam para imprimir todos os records existentes no diretorio
int readRecordsDir(int recordsDir){
	
	read_block(2, genericBlock);
	t2fs_record blockOfRecords[16];
	int i, primeiroRecordInvalido=99;
	for(i=0; i < 16; i++){
            memcpy(&blockOfRecords[i], &genericBlock[i*sizeof(t2fs_record)], sizeof(t2fs_record));
            /*if (primeiroRecordInvalido == 99 && blockOfRecords[i].TypeVal != 1 &&blockOfRecords[i].TypeVal != 2)
				primeiroRecordInvalido = i;
			printf("Tipo: %i\n", blockOfRecords[i].TypeVal);
			printf("name: %s\n", blockOfRecords[i].name);
			printf("blocksFileSize: %i\n", blockOfRecords[i].blocksFileSize);
			printf("bytesFileSize: %i\n", blockOfRecords[i].bytesFileSize);
			printf("dataPtr[0]: %i\n", blockOfRecords[i].dataPtr[0]);
			printf("dataPtr[1]: %i\n", blockOfRecords[i].dataPtr[1]);
			printf("singleIndPtr: %i\n", blockOfRecords[i].singleIndPtr);
			printf("doubleIndPtr: %i\n\n", blockOfRecords[i].doubleIndPtr);*/
			}
	return primeiroRecordInvalido;
}

t2fs_record getRecord(char *nome, t2fs_record directoryRec){
	
	int i=0,index = 0, currentBlock=3;
	//char blocoLido[superbloco.BlockSize];
	int newRegister = 1; //se o registro recem entrou no while entao deve ser incrementado o currentBLock
	char backupNome[200];
	strcpy(backupNome, nome);
	deleteFirstPathDir(nome);
	strcpy(backupNome, nome);
	getFirstPathDir(nome); //nameaux possui agora o nome do arquivo abaixo do root
	DWORD blocksFileAux = directoryRec.blocksFileSize;
	blocksFileAux = 5;
	t2fs_record emptyReg; //usado para ser retorno onde funcao retornaria record, mas nao o acharam;portanto retorna um reg invalido
	emptyReg.TypeVal = 0xFF;
	
	t2fs_record aux_record = directoryRec;
	
	while(currentBlock < blocksFileAux){
		//read_block(getNextReadableBlock(aux_record, currentBlock), genericBlock);
		//printf("%d",getNextReadableBlock(aux_record, currentBlock));
		read_block(2, genericBlock);
		t2fs_record blockOfRecords[16];
		newRegister = 0;
		//alocar os registros lidos do bloco em um vetor de estruturas de registros
		for(i=0; i < 16; i++){
			memcpy(&blockOfRecords[i], &genericBlock[i*sizeof(t2fs_record)], sizeof(t2fs_record)); 
			if(!strcmp(backupNome, blockOfRecords[i].name)){//achou o registro!!!!
				if(!strcmp(deleteFirstPathDir(nome),backupNome)){
					return blockOfRecords[i]; //se nao ha mais nada pra remover do path eh pq eh o ultimo
				}
				else{
					aux_record = blockOfRecords[i];
					newRegister = 1;
					}
				index = i;
				break;
			}
		}
		if(newRegister){
			blocksFileAux = blockOfRecords[index].blocksFileSize;
			currentBlock = 1;
			deleteFirstPathDir(backupNome);
		}
		else currentBlock++;
	}
	return emptyReg; //retorna um reg invalido global
}


int getDirectoryPositionByName(t2fs_record directoryRec, char* name){
	int currentBlock = 1;// checa-se se existe o bloco currentBlock no registro do diretorio
	int i = 0; //iterator
	int blockToCheck=0;
	while(blockToCheck!=0xFFFFFFFF){
		blockToCheck = getNextReadableBlock(directoryRec, currentBlock);
		if (blockToCheck != 0xFFFFFFFF){ 
			//bloco precisa ser checado para existencia  
			read_block(blockToCheck, genericBlock);
			for(i=0; i < 16; i++){
				memcpy(&root[i], &genericBlock[i*sizeof(t2fs_record)], sizeof(t2fs_record)); //copia o bloco em um vetor de records
				if(root[i].name == name) 
					return ((1024 * (currentBlock-1))+i); //retorna o offset para o local em que o record esta
			}
		}
		currentBlock++;
	}
	return 0;
}


//retorna o record do diretorio pai inteiro de um arquivo com link absoluto
t2fs_record getParentDirectory(char *nome){
      char *p;
      char parentDir[39]; //parentDir vai receber o full path do parent directory
      p = strrchr(nome, '/');
      if (p && p != nome+1){
          *p = 0;
          p = strrchr(p-1, '/');
          if (p){
              strcpy(parentDir, p+1);  
              }
          else{
              strcpy(parentDir, nome); 
              }
      }
      return getRecord(parentDir, superbloco.RootDirReg);
}

void updateRecord(t2fs_record parentDir, t2fs_record changedRec, DWORD location){
	DWORD dirBlock = 	getNextReadableBlock(parentDir, location/1024);
	read_block(dirBlock, genericBlock);
	int i;
	for(i=0; i < 16; i++)
		memcpy(&root[i], &genericBlock[i*sizeof(t2fs_record)], sizeof(t2fs_record)); //copia o bloco em um vetor de records
	root[location%1024] = changedRec;
	for(i=0; i < 16; i++)
		memcpy( &genericBlock[i*sizeof(t2fs_record)], &root[i], sizeof(t2fs_record));
	write_block(dirBlock, genericBlock);
}

//retorna o nome do arquivo (posicao mais a direita do path)
char *getfileName(char *path){
	char *token = '\0';
	deleteFirstPathDir(path);
	token = strtok(path, "/");
	token = strtok(NULL, "/");
	if(token == NULL){ return path;
	}
	else return getfileName(path);
}
