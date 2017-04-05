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
  ans = ftell(f); // Esse ftell ta certo.
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
  ull offs, offs2, data;
  theader_t* t = readHeader(nfield);
  int i, aux[MAX], count;
  char buf[MAX][nfield], c, aux2;

  f = fopen("arquivo.dat","a+");
  if(f == NULL){
    printf("Arquivo não encontrado\n");
    exit(0);
  }

  fseek(f, 0, SEEK_END); // Comeca no fim do cabecalho
  offs2 = offs = ftell(f); //Offset inicial dos offsets
  data = offs + nfield * sizeof(ull); // offset inicias dos dados
  for(i = 0; i < nfield; i++){ // salva todos os offsets dos offsets primeiro e armazena a data em algum lugar
    fseek(f, offs, SEEK_SET);
    fwrite(&data, sizeof(ull), 1, f);
    printf("O lugar em que eu vou escrever eh %llu\n", data);
    switch(t[i].type){
      case 'S':
      for(int s=0;(aux2=getchar())!='\n'; s++){
        buf[i][s]=aux2; // O PROBLEMA ESTA AQUI
        for(int r = 0; r < nfield; r++){
          for(int s=0;s < 10; s++) printf("%c ", buf[r][s]); // O PROBLEMA ESTA AQUI
          printf("\n");
        }
      }
      // for(int r = 0; r < nfield; r++){
      //   for(int s=0;s < 10; s++) printf("%c ", buf[r][s]); // O PROBLEMA ESTA AQUI
      //   printf("\n");
      // }
      printf("O que eu li foi %s e escrevi em %llu\n", buf[i], data);
      data += strlen(buf[i]);
      break;
      case 'C':
      scanf("%c", &buf[i][0]);
      while((c = getchar()) != '\n' && c != EOF);
      fwrite(buf[i], 1, 1, f);
      data++;
      break;
      case 'I':
      scanf("%d", &aux[i]);
      while((c = getchar()) != '\n' && c != EOF);
      data += sizeof(int);
      break;
    }
    offs += sizeof(ull);
  }
  for(i = 0; i < nfield; i++){
    fseek(f, offs2, SEEK_SET);
    fread(&data, sizeof(ull), 1, f);
    printf("o offset em que estou escrevendo é %llu\n", data);
    fseek(f, data, SEEK_SET);
    switch(t[i].type){
      case 'S':
      printf("to escrvendo na bagaca %s\n", buf[i]);
      for(int s=0;buf[i][s]!='\0'; s++){
        fwrite(&buf[i][s], 1, 1, f);
      }
      //fwrite(buf[i], strlen(buf[i]), 1, f);
      break;
      case 'C':
      fwrite(buf[i], 1, 1, f);
      break;
      case 'I':
      fwrite (&aux[i], sizeof(int), 1, f);
      break;
    }
    offs2 += sizeof(ull);
  }
  fclose(f);
} //Insere uma tupla

void selectAll(int nfield, ull hEnd){
  FILE *f;
  theader_t *t;
  char buf[MAX];
  tint num;
  ull off, aux, aux2, comparator;
  int i, j;
  bool flag=0;
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
  printf("%llu\n", hEnd);
  fseek(f, 0, SEEK_END);
  aux = ftell(f);
  while(flag==0){ // Enquanto o arquivo não terminar
    for(i = 0; i < nfield; i++, hEnd += sizeof(ull)){
      fseek(f, hEnd, SEEK_SET);
      fread(&off, sizeof(ull), 1, f);
      fseek(f, hEnd+sizeof(ull), SEEK_SET);
      fread(&aux2, sizeof(ull), 1, f);
      if(aux2 < aux) comparator = aux2;
      else{
        comparator = aux;
        flag = 1;
      }
      fseek(f, off, SEEK_SET);
      switch(t[i].type){
        case 'S':
        for(j = 0; ftell(f) < comparator; j++)
        fread(&buf[j], 1, 1, f);
        //buf[j] = 0;
        printf("%s", buf);
        break;
        case 'C':
        fread(buf, 1, 1, f);
        printf("%c", buf[0]);
        break;
        case 'I':
        for(j = 0; ftell(f) < comparator; j++) fread(num.cint, 1, 1, f);
        printf("%d", num.vint);
        break;
      }
      printf(" | ");
    }
    printf("\n");
  }
} //Imprime todas as tuplas
