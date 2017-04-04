//#()&*
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX 11234

typedef struct theader{
  char name[MAX];
  char type;
}theader_t;

int buildHeader();
theader_t* readHeader(int nfield);
void insert(int nfield);

int main(void){
  int nfield;

  nfield = buildHeader();
  insert(nfield);
  return 0;
}

int buildHeader(){ //Constrói o header do arquivo
  int i, nfield, flen;
  char fname[MAX], ftype;
  FILE *f;

  f = fopen("arquivo.dat", "w+"); //abre o arquivo e trata falhas
  if(f == NULL){
    printf("Arquivo não pôde ser criado\n");
    exit(0);
  }

  printf("Insira o numero de campos\n");
  scanf("%d", &nfield);//Le a quantidade de campos
  for(i = 0; i < nfield; i++){ //Le e escreve nome, tipo e tamanho do atrib.
    printf("Insira o nome do atributo #%d\n", i+1);
    scanf("%s", fname);
    getchar();
    printf("Insira o tipo do atributo #%d\n", i+1);
    scanf("%c", &ftype);
    flen = strlen(fname);
    fwrite(&ftype, 1, 1, f);
    fwrite(&flen,sizeof(int),1,f);
    fwrite(fname, flen, 1, f);
  }
  strcpy(fname,"#"); //header encerrado com #
  fwrite(fname, 1, 1, f);
  fclose(f);
  return nfield;
}

theader_t* readHeader(int nfield){
  FILE *f;
  theader_t *t = (theader_t*) malloc(sizeof(theader_t) * nfield);
  int i, tmp;
  char ftype;

  f = fopen("arquivo.dat", "r");
  if(f == NULL){
      printf("Arquivo não encontrado\n");
      exit(0);
  }
  for(i = 0; i < nfield; i++){
      fread(&ftype, 1, 1, f);
      if(ftype == '#') break;
      t[i].type = ftype;
      fread(&tmp, sizeof(int), 1, f);
      fread(t[i].name, tmp, 1, f);
printf("Campo: %s tipo: %c\n",t[i].name, t[i].type);
  }
  return t;
}

void insert(int nfield){
  FILE *f;
  theader_t* t = readHeader(nfield);
  f = fopen("arquivo.dat","a+");
  if(f == NULL){
    printf("Arquivo não encontrado\n");
    exit(0);
  }

  fclose(f);
}
