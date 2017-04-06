/* UNIVERSIDADE FEDERAL DA FRONTEIRA SUL;
DEPARTAMENTO DE CIENCIA DA COMPUTAÇÃO - CAMPUS CHAPECO;
CÓDIGO MIRACULOSAMENTE DESENVOLVIDO POR FELIPE CHABATURA E LEONARDO TIRONI;
*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX 11234

typedef unsigned long long ull;

typedef struct theader{ //Struct utilizada para ler o header
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
  bool leave=0;
  char FUNCIONOU_CARAI;
  printf("== CREATE TABLE ==\n");
  printf("Insira o numero de campos\n");
  scanf("%d", &nfield);//Le a quantidade de campos
  hEnd = buildHeader(nfield);
  while(leave==0){
    // printf("O QUE DESEJA?\n");
    // printf("[I]nserir Dado\n");
    // printf("I[m]primir Tudo\n");
    // printf("[S]air\n");
    FUNCIONOU_CARAI=getchar();
    getchar();
    switch(FUNCIONOU_CARAI){
      case 'I':
      insert(nfield); // Inserir um campo
      break;
      case 'm':
      selectAll(nfield, hEnd);
      break;
      case 'S':
      leave = 1;
      break;
    }
  }
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

   //Le e escreve tipo, tamanho e nome de cada atrib. no arquivo
  for(i = 0; i < nfield; i++){
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
  //salva e retorna o final do header
  ans = ftell(f);
  fclose(f);
  return ans;
}

theader_t* readHeader(int nfield){ //Le o Header num vetor de theaders
  FILE *f;
  theader_t *t = (theader_t*) malloc(sizeof(theader_t) * nfield);
  int i, tmp;
  char ftype;

  f = fopen("arquivo.dat", "r"); //abre o arquivo e testa por erros
  if(f == NULL){
    printf("Arquivo não encontrado\n");
    exit(0);
  }
  for(i = 0; i < nfield; i++){ //Le o tipo, tamanho e nome dos atrib.
    fread(&ftype, 1, 1, f);
    if(ftype == '#') break;
    t[i].type = ftype;
    fread(&tmp, sizeof(int), 1, f);
    fread(t[i].name, tmp, 1, f);
  }
  fclose(f);
  return t;
}

void insert(int nfield){ //Insere uma tupla no arquivo
  FILE *f;
  ull offs, offs2, data;
  theader_t* t = readHeader(nfield);
  int i, aux, s;
  char buf[nfield][MAX], c, aux2;

  f = fopen("arquivo.dat","r+");//Abre o arquivo e testa por erros
  if(f == NULL){
    printf("Arquivo não encontrado\n");
    exit(0);
  }
  fseek(f, 0, SEEK_END);
  ull o = ftell(f)-1;
  fread(&c, 1, 1, f);
  if(c != '$') fseek(f, o+1, SEEK_SET);
  else fseek(f, o, SEEK_SET);

  offs2 = offs = ftell(f); //offset inicial dos offsets
  data = offs + nfield * sizeof(ull); //offset inicial dos dados
  // salva todos os offsets dos dados primeiro e armazena a data no buffer
  for(i = 0; i < nfield; i++){
    fseek(f, offs, SEEK_SET); // va para onde eu quero escrever o proximo offstet
    fwrite(&data, sizeof(ull), 1, f);//Escreve o proximo offset
    printf("\n%s:",t[i].name);
    switch(t[i].type){//Le o atributo em questao e o escreve no arquivo
      case 'S':
        for(s=0;(aux2=getchar())!='\n'; s++) buf[i][s]=aux2;
        buf[i][s]='\0';
        data += strlen(buf[i]);
        break;
      case 'C':
        scanf("%c", &buf[i][0]);
        while((c = getchar()) != '\n' && c != EOF); // garbage collector
        data++;
        break;
      case 'I':
        scanf("%d", &aux);
        while((c = getchar()) != '\n' && c != EOF); // garbage collector
        data += sizeof(int);
        break;
    }
    offs += sizeof(ull);//vai para o offset do proximo offset a ser lido
  }
  for(i = 0; i < nfield; i++){ //Escreve os dados do buffer no arquivo
    fseek(f, offs2, SEEK_SET); //Coloca o buffer de leitura no offset a ser lido
    fread(&data, sizeof(ull), 1, f);//Le o offset atual
    fseek(f, data, SEEK_SET); //Coloca o buffer no local de escrita atual
    switch(t[i].type){ //Escreve o dado no arquivo
      case 'S':
      fwrite(buf[i], strlen(buf[i]), 1, f);
      break;
      case 'C':
      fwrite(&buf[i][0], 1, 1, f);
      break;
      case 'I':
      fwrite (&aux, sizeof(int), 1, f);
      break;
    }
    offs2 += sizeof(ull);
  }
  c = '#'; //Caractere especial no fim da linha
  fwrite(&c, 1, 1, f);
  c = '$'; //Caractere especial no fim do arquivo
  fwrite(&c, 1, 1, f);
  fclose(f);
}

void selectAll(int nfield, ull hEnd){
  FILE *f;
  theader_t *t;
  char buf[MAX], c = 'a';
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
  // fseek(f, 0, SEEK_END);
  // aux = ftell(f);
  while(c != '$'){ // Enquanto o arquivo não terminar
    for(i = 0, c = 'a'; i < nfield; i++, hEnd += sizeof(ull)){
      fseek(f, hEnd, SEEK_SET); //Le o proximo offset
      fread(&off, sizeof(ull), 1, f);
      if(i == nfield - 1){
        fseek(f, off, SEEK_SET);
        while(c != '#') fread(&c, 1, 1, f);
        comparator = ftell(f) - 1;
      }
      else{
        fseek(f, hEnd+sizeof(ull), SEEK_SET);
        fread(&aux2, sizeof(ull), 1, f);
        comparator = aux2;
      }
      fseek(f, off, SEEK_SET);
      switch(t[i].type){
        case 'S':
        for(j = 0; ftell(f) < comparator; j++) fread(&buf[j], 1, 1, f);
        buf[j] = '\0';
        printf("%s", buf);
        break;
        case 'C':
        fread(&buf[0], 1, 1, f);
        printf("%c", buf[0]);
        break;
        case 'I':
        for(j = 0; ftell(f) < comparator; j++) fread(&num.cint[j], 1, 1, f);
        printf("%d", num.vint);
        break;
      }
      printf(" | ");
    }
    printf("\n");
    fread(&c, 1, 1, f);
    ull n = ftell(f);
    fread(&c, 1, 1, f);
    if(c != '$') hEnd=n;
  }
} //Imprime todas as tuplas
