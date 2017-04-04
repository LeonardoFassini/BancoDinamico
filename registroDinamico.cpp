//#()&*!
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX 11234

typedef unsigned long long ull;

typedef struct theader{
  char name[MAX];
  char type;
}theader_t;

typedef union tint{
  char cint[sizeof(int)];
  int vint;
} tint;

ull buildHeader(int nfield);
theader_t* readHeader(int nfield);
void insert(int nfield);
void selectAll(int nfield, ull hEnd);

int main(void){
  int nfield;
  ull hEnd;

  printf("Insira o numero de campos\n");
  scanf("%d", &nfield);//Le a quantidade de campos
  hEnd = buildHeader(nfield);
  insert(nfield);
  selectAll(nfield, hEnd);
  return 0;
}

ull buildHeader(int nfield){ //Constrói o header do arquivo
  int i, flen;
  char fname[MAX], ftype;
  ull ans;
  FILE *f;

  f = fopen("arquivo.dat", "w+"); //abre o arquivo e trata falhas
  if(f == NULL){
    printf("Arquivo não pôde ser criado\n");
    exit(0);
  }

  for(i = 0; i < nfield; i++){ //Le e escreve nome, tipo e tamanho do atrib.
    printf("Insira o nome do atributo #%d\n", i+1);
    scanf("%s", fname);
    getchar();
    printf("Insira o tipo do atributo #%d\n", i+1);
    scanf("%c", &ftype);
    getchar();
    flen = strlen(fname);
    fwrite(&ftype, 1, 1, f);
    fwrite(&flen,sizeof(int),1,f);
    fwrite(fname, flen, 1, f);
  }
  strcpy(fname,"#"); //header encerrado com #
  fwrite(fname, 1, 1, f);
  ans = ftell(f);
  fclose(f);
  return ans;
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
  }
  fclose(f);
  return t;
} // Cria um vetor com os nomes e tipos de cada atributo

void insert(int nfield){
  FILE *f;
  ull* offsets;
  ull offs, data;
  offsets = (ull*)malloc(sizeof(ull) * nfield);
  theader_t* t = readHeader(nfield);
  int i, num;
  char buf[MAX], c;

  f = fopen("arquivo.dat","a+");
  if(f == NULL){
    printf("Arquivo não encontrado\n");
    exit(0);
  }
  offs = ftell(f);
  data = offs + nfield * sizeof(ull);
  fwrite(&offs, sizeof(ull), nfield, f);
  for(i = 0; i < nfield; i++){
    fseek(f, offs, SEEK_SET);
    fwrite(&data, sizeof(ull), 1, f);
    fseek(f, data, SEEK_SET);
    printf("\n%s :",t[i].name);
    switch(t[i].type){
      case 'S':
        fgets(buf, MAX,stdin);
        buf[strlen(buf)-1] = 0;
        fwrite(buf, strlen(buf), 1, f);
        data += strlen(buf);
        break;
      case 'C':
        scanf("%c", &buf[0]);
        while((c = getchar()) != '\n' && c != EOF);
        fwrite(buf, 1, 1, f);
        data++;
        break;
      case 'I':
        scanf("%d", &num);
        while((c = getchar()) != '\n' && c != EOF);
        fwrite (&num, sizeof(int), 1, f);
        data += sizeof(int);
        break;
    }
    offs += sizeof(ull);
  }
  fclose(f);
} //Insere uma tupla

void selectAll(int nfield, ull hEnd){
  FILE *f;
  theader_t *t;
  char buf[MAX];
  tint num;
  ull off;
  int i, j;

  t = readHeader(nfield); //Le o header
  f = fopen("arquivo.dat", "r"); // abre o arquivo
  if(f == NULL){
    printf("Arquivo não encontrado\n");
    exit(0);
  }
  for(i = 0; i < nfield; i++){ // Imprime o nome de todos os atributos
    printf("%s | ",t[i].name);
  }
  printf("\n");
  //printf("%llu\n", hEnd);
  while(!feof(f)){ // Enquanto o arquivo não terminar
    for(i = 0; i < nfield; i++, hEnd += sizeof(ull)){
      fseek(f, hEnd, SEEK_SET);
      fread(&off, sizeof(ull), 1, f);
      fseek(f, off, SEEK_SET);
      switch(t[i].type){
        case 'S':
          for(j = 0; ftell(f) < off + sizeof(ull); j++)
            fread(&buf[j], 1, 1, f);
            //buf[j] = 0;
          printf("%s", buf);
          break;
        case 'C':
          fread(buf, 1, 1, f);
          printf("%c", buf[0]);
          break;
        case 'I':
          for(j = 0; ftell(f) < off + sizeof(ull); j++)
            fread(num.cint, 1, 1, f);
          printf("%d", num.vint);
          break;
      }
      printf(" | ");
    }
    printf("\n");
  }
} //Imprime todas as tuplas
