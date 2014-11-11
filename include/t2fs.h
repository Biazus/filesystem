

#ifndef __LIBT2FS___
#define __LIBT2FS___

#define TYPEVAL_REGULAR     0x01
#define TYPEVAL_DIRETORIO   0x02
#define TYPEVAL_INVALIDO    0xFF

typedef int FILE2;
typedef int DIR2;

typedef unsigned char BYTE;
typedef unsigned short int WORD;
typedef unsigned int DWORD;

/** Registro de diretório (entrada de diretório) */
struct t2fs_record {
    BYTE    TypeVal;        /* Tipo da entrada. Indica se o registro é inválido (0xFF), arquivo (0x01 ou diretório (0x02) */
    char    name[31];       /* Nome do arquivo. : string com caracteres ASCII (0x21 até 0x7A), case sensitive. */
    DWORD   blocksFileSize; /* Tamanho do arquivo, expresso em número de blocos de dados */
    DWORD   bytesFileSize;  /* Tamanho do arquivo. Expresso em número de bytes. */
    DWORD   dataPtr[4];     /* Quatro ponteiros diretos, para blocos de dados do arquivo */
    DWORD   singleIndPtr;   /* Ponteiro de indireção simples */
    DWORD   doubleIndPtr;   /* Ponteiro de indireção dupla */
} __attribute__((packed));

/** Superbloco */
struct t2fs_superbloco {
    char    Id[4];          /* Identificação do sistema de arquivo. É formado pelas letras “T2FS”. */
    WORD    Version;        /* Versão atual desse sistema de arquivos: (valor fixo 7DE=2014; 2=2 semestre). */
    WORD    SuperBlockSize; /* Quantidade de setores  que formam o superbloco. (fixo em 1 setor) */
    DWORD   DiskSize;       /* Tamanho total da partição T2FS, incluindo o tamanho do superblock. */
    DWORD   NofBlocks;      /* Quantidade total de blocos de dados na partição T2FS (1024 blocos). */
    DWORD   BlockSize;      /* Tamanho de um bloco.*/
    char    Reserved[108];  /* Não usados */
    struct t2fs_record BitMapReg;  /* Registro que descreve o arquivo que mantém o bitmap de blocos livres e ocupados */
    struct t2fs_record RootDirReg; /* Registro que descreve o arquivo que mantém as entradas do diretório raiz */
} __attribute__((packed));


#define MAX_FILE_NAME_SIZE 255
typedef struct {
    char name[MAX_FILE_NAME_SIZE+1];
    int fileType;   // ==1, is directory; ==0 is file
    unsigned long fileSize;
} DIRENT2;

#endif
