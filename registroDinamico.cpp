#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX 11234

void buildHeader(){
  int i, nfield, flen;
  char fname[MAX], ftype;
  FILE *f;

  f = fopen("arquivo.dat", "w+");
  if(f == NULL){
    printf("Arquivo não pôde ser criado\n");
    exit(0);
  }

  printf("Insira o numero de campos\n");
  scanf("%d", &nfield);
  for(i = 0; i < nfield; i++){
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
  strcpy(fname,"#");
  fwrite(fname, 1, 1, f);
  fclose(f);
}

int main(void){
  buildHeader();

  return 0;
}
