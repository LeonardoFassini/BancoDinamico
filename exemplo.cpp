#include <stdio.h>
#include <stdlib.h>
#include <string.h>



#define MFIELD 10 // Maximo de Campos

void buildHeader()
{
	char fname[15], ftype; // Nome do atributo a ser inserido e tipo do atributo
	int flen; // tamanho do atributo
	FILE *f; // arquivo
	f=fopen("agenda.dat","w+"); // abra o documento em modo de leitura e atualizacao
	if (f==NULL) // tratamento se nao abrir
	{
		printf("File could not be created\n");
		exit(0);
	}
	/// first field (attribute Name)
	strcpy(fname,"codigo"); // Escreva em fname "codigo"
	ftype='S'; // isso quer dizer que eu vou escrever no arquivo uma string.
	flen=5; // tamanho maximo dos atributos que ele poderá receber
	//Escreva o que esta em fname, com tamanho maximo do atributo 15, uma vez em f
	fwrite(fname,15,1,f);
	fwrite(&ftype,1,1,f); //Escreva S em f
	fwrite(&flen,sizeof(int),1,f); //Escreva o tamanho no f
	/////
	strcpy(fname,"nome");
	ftype='S';
	flen=20;
	fwrite(fname,15,1,f);
	fwrite(&ftype,1,1,f);
	fwrite(&flen,sizeof(int),1,f);

	strcpy(fname,"idade");
	ftype='I';
	flen=sizeof(int);
	fwrite(fname,15,1,f);
	fwrite(&ftype,1,1,f);
	fwrite(&flen,sizeof(int),1,f);

	strcpy(fname,"#");
	// Coloca #, com tamanho de 20 bytes, para manter todos os atributos com o
	// mesmo tamanho, faz isso 7 vezes para chegar ao MFIELD maximo;
	fwrite(fname,15+1+sizeof(int),MFIELD-3,f);
	fclose(f);
}

struct name_t {
	/* data */
};

struct theader { //Uma das tuplas
	char name[15];
	char type;
	int  len;
};

union tint {
	char cint[sizeof(int)];
	int vint;
};

////
struct theader* readHeader()
{
	FILE *f;
	struct theader *th=(struct theader*) malloc(sizeof(struct theader)*MFIELD); // Aloca um vetor de 10 structs
	int i;
	////
	f=fopen("agenda.dat","r"); // Abra em modo leitura a agenda.dat
	if (f==NULL)
	{
		printf("File not found\n");
		exit(0);
	}
	///
	for (i=0; i<MFIELD ;i++) // enquanto i < o tamanho maximo de colunas da header
	{
		if(i>0 && th[i-1].name[0]=='#') break;
		fread(th[i].name,15,1,f); //le as paradas
		fread(&th[i].type,1,1,f);
		fread(&th[i].len,sizeof(int),1,f);
	}
	fclose(f);
	return th;
}

void insert()
{
	FILE *f;
	struct theader *t;
	int i;
	char opt, buf[100],c;
	union tint eint;
	////
	t=readHeader(); // pega um vetor de atributos, sendo esse vetor o cabecalho.
	f=fopen("agenda.dat","a+"); // abra o arquivo e de permissão para atualizar ele.
	if (f==NULL)
	{
		printf("File not found\n");
		exit(0);
	}
	do {
		i=0;
		while (i<10 && t[i].name[0]!='#') // entquanto nao ultrapassar nenhum atributo
		{
			printf("\n%s :",t[i].name);
			switch (t[i].type)
			{
				//leia do teclado um maximo do tamanho permitido pelo header e coloque num buffer
				case 'S': fgets(buf,t[i].len+1,stdin);
				if (buf[strlen(buf)-1]!='\n')  c = getchar();
				else buf[strlen(buf)-1]=0;
				fwrite(buf,t[i].len,1,f);
				break;
				case 'C': buf[0]=fgetc(stdin);
				while((c = getchar()) != '\n' && c != EOF); /// garbage collector
				fwrite(buf,t[i].len,1,f);
				break;
				case 'I': scanf("%d",&eint.vint);
				while((c = getchar()) != '\n' && c != EOF); /// garbage collector
				fwrite (&eint.vint,t[i].len,1,f);
				break;
			}



			i++;
		}

		printf("Continuar (S/N): "); opt=getchar();
		while((c = getchar()) != '\n' && c != EOF); /// garbage collector

	} while (opt=='S' || opt=='s');
	fclose(f);
}

void selectAll()
{
	FILE *f;
	struct theader *t;
	int hlen,i,j,space;
	char buf[100];
	union tint eint;
	////
	t=readHeader();
	f=fopen("agenda.dat","r");
	if (f==NULL)
	{
		printf("File not found\n");
		exit(0);
	}

	i=0;
	while (i<10 && t[i].name[0]!='#')
	{
		printf("%s ",t[i].name); //Imprima o nome
		space=t[i].len-strlen(t[i].name); // Imprima espacos ate chegar ao espacamento
		for (j=1;j<=space;j++)
		printf(" ");
		i++;
	}
	printf("\n");
	///
	hlen=MFIELD*(15+1+sizeof(int));
	/// skip file's header
	fseek(f,hlen,SEEK_SET);
	do {
		i=0;
		while (i<10 && t[i].name[0]!='#')
		{
			if (!fread(buf,t[i].len,1,f)) break;
			switch (t[i].type)
			{
				case 'S':
				for (j=0;j<t[i].len && buf[j]!=0;j++) printf("%c",buf[j]);
				space=t[i].len-j;
				for (j=0;j<=space;j++)
				printf(" ");
				break;
				case 'C': printf("%c ",buf[0]);
				break;
				// como o union divide a posicao na memoria entre duas variaveis de mesmo
				// tamanho em bytes, o cint recebe um BYTE por vez do inteiro, depois
				// ele printa os 4 bytes em forma de inteiro.
				case 'I': for (j=0;j< t[i].len;j++) eint.cint[j]=buf[j];
				printf("%d",eint.vint);
				break;
			}
			i++;
		}
		printf("\n");
	} while (!feof(f));
}

int main()
{
	buildHeader();
	insert();
	selectAll();

	return 0;
}
