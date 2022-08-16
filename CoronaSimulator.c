/*	
	Compilador GCC;
	SO: Deepin 15.11;
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

typedef struct familia{
	
	char nome[2];
	unsigned int pessoas;
	short int doente;	// doente == 0 NÃO ESTA DOENTE; 
						// doente == -1 ESTA DOENTE;
	struct familia *prox;
	struct liga *list;
	unsigned int nFamilias;

}Familia;

typedef struct liga{

	Familia *familia;
	struct liga *prox;

}Ligacao;

typedef struct cabecaFamilia{
	
	int qtd;
	int nPessoas;
	Familia *primeiro;
	Familia *ultimo;

}Head;

typedef struct virus{

	Familia *target;
	int mov;
	struct virus *prox;
	struct virus *ante;

}Virus;

typedef struct cabecaVirus{
	
	int qtd;
	Virus *primeiro;
	Virus *ultimo;

}HeadV;

void print_log(FILE *log, char *msg);	// (ponteiro pro log, "Mensagem");
void erro_log(FILE *log, char *msg);	// (ponteiro pro log, "mensagem");
Familia *insereFamilia(char *n, int qtd);
void move_virus(FILE *log, Virus *virus);
Familia *busca(Familia *no, char *str);
int ligafamilias(Familia *f1, Familia *f2);
Virus *aloca_Virus(Familia *alvo, Virus *back, Virus *next);
int agente_atua(FILE *log, Familia *f, HeadV *head);
void agente_saude(FILE *log, Familia *f);
void verifica_status(Familia *f, unsigned short *v, int media);



void main(void ){

	FILE *arq, *log;

	char funcao[17], nf1[2], nf2[2];
	int parametro, valLiga=1, acertos=0, erros=0, bkpVirus, mda80; //valLiga == 1 pq nao houve ligacao;
	int setMov[3]={0,0,0};	//0 - agente_atua; 
							//1 - virus_multiplica;
							//2 - iniciasimulacao;
	unsigned short status[2]={0,0};	// 0, 0 = Nao houve surto
									//1, 0 = Houve surto masi foi controlado
									//1, 1 = Estado de emergencia

	Head headFamilia;
	HeadV headVirus;

	headFamilia.qtd=0;
	headFamilia.nPessoas=0;
	headFamilia.primeiro=NULL;
	headFamilia.ultimo=NULL;

	headVirus.qtd=0;
	headVirus.primeiro=NULL;
	headVirus.ultimo=NULL;

	arq = fopen("entrada.txt", "r");
	log = fopen("log.txt", "w"); //W para apagar o log antigo caso exista;

	if(arq == NULL)
		erro_log(log, "Erro na abertura do arquivo");

	
	while(!feof(arq)){

		fscanf(arq,"%s",funcao);

		if(!(strcmp(funcao,"agente_atua"))){
			//printf("IF agente_atua\n");
			fscanf(arq,"%d", (setMov));
			if(*(setMov)<0)
				erro_log(log, "Valor invalido passado no agente_atua");
			print_log(log, "set agente_atua com sucesso!");
		}

		if(!(strcmp(funcao,"virus_multiplica")) || !(strcmp(funcao,"vírus_multiplica"))){
			//printf("IF virus_multiplica\n");
			fscanf(arq,"%d", (setMov+1));

			if(*(setMov+1)<0)
				erro_log(log, "Valor invalido passado no virus_multiplica");
			print_log(log, "set virus_multiplica com sucesso!");
		}

		if(!(strcmp(funcao,"inserefamilia"))){

			//printf("IF inserefamilia\n");
			fscanf(arq,"%2s",nf1);
			fscanf(arq,"%d", &parametro);

			Familia *aux=NULL;

			aux=busca(headFamilia.primeiro, nf1);

			if(aux != NULL){	//Ja existe essa familia
				char msglog[22]="A familia ";
				strcat(msglog, nf1);
				strcat(msglog, " ja existe");
				erro_log(log, msglog);
			}

			aux = insereFamilia(nf1, parametro);

			if(aux == NULL)	erro_log(log, "O sistema operacional nao liberou espaco!");

			if(headFamilia.primeiro == NULL){
				headFamilia.primeiro=aux;
				headFamilia.ultimo=aux;
			}else{
				headFamilia.ultimo->prox=aux;
				headFamilia.ultimo = aux;
			}

			headFamilia.nPessoas = headFamilia.nPessoas + parametro;
			headFamilia.qtd++;

			char msglog[22]="Familia adicionada: " ;

			strcat(msglog, nf1);

			print_log(log, msglog);
		}

		if(!(strcmp(funcao,"ligafamilias"))){
		//	printf("===IF ligafamilias===\n");

			if(!(headFamilia.qtd))	//Confere c ha familia
				erro_log(log, "Nao ha familias");

			valLiga=0;
			Familia *aux, *aux1;
			char auxNf1[2];

			fscanf(arq, "%2s", &nf1);
            strcpy(auxNf1,nf1);
			fscanf(arq, "%2s", &nf2);

			aux = busca(headFamilia.primeiro, auxNf1);
			aux1 = busca(headFamilia.primeiro, nf2);

			if(!(strcmp(auxNf1, nf2)))
				erro_log(log, "Voce nao pode ligar a familia nela mesma");

			if(aux == NULL || aux1 == NULL){

				char msglog[45] = "Erro em ligafamilias, familia inexistente: ";

				(aux == NULL) ? strcat(msglog, auxNf1) : strcat(msglog, nf2);

				erro_log(log, msglog);
			}

			int valida=ligafamilias(aux, aux1);			
				
			if(valida>1){//Sem erros
				char msglog[80]="Set ligafamilias ";
				
				strcat(msglog, auxNf1);
				strcat(msglog, " com ");
				strcat(msglog, nf2);

				print_log(log, msglog);
			}else if(valida == 1)	//Familias ja ligadas
				erro_log(log, "Erro na ligafamilias, tentando ligar familias ja ligadas");
			else
				erro_log(log, "Erro na ligafamilias, o sistema nao liberou espaco");
		}

		if(!(strcmp(funcao,"inserevirus"))){
		//	printf("===IF inserevirus===\n");
			fscanf(arq, "%2s", &nf1);

			Familia *aux = busca(headFamilia.primeiro, nf1);

			if(aux == NULL){
				char msglog[41]="Erro inserevirus, familia inexistente: ";
				strcat(msglog, nf1);
				erro_log(log, msglog);
			}
		
			if(headVirus.primeiro == NULL){
				Virus *auxVirus=aloca_Virus(aux, NULL, NULL);
				headVirus.primeiro=auxVirus;
				headVirus.ultimo=auxVirus;
			}else{
				Virus *auxHead, *auxVirus=aloca_Virus(aux, headVirus.ultimo, NULL);
				auxHead=headVirus.ultimo;
				auxHead->prox=auxVirus;
				headVirus.ultimo=auxVirus;
			}

			char msglog[28]="Virus inserido na familia ";
			strcat(msglog, nf1);
			print_log(log, msglog);

			headVirus.qtd++;
		}

		if(!(strcmp(funcao,"iniciasimulacao"))){

			// REVISA TD ANTES DE INICIAR

			bkpVirus=headVirus.qtd; //Para printat dps no log
			mda80 = (headFamilia.nPessoas * 80) / 100;	//Calcula quanto é 80%

			if(!(headFamilia.qtd))
				erro_log(log, "Nao ha familias");

			if(valLiga)
				erro_log(log, "Nenhuma familia esta ligada");
			else{
				Familia *f = headFamilia.primeiro;

				while(f != NULL){
					if(!(f->nFamilias)){ //nFamilias == 0
						char msglog[]="A familia ";
						strcat(msglog, f->nome);
						strcat(msglog, " nao tem ligacao com nenhuma outra");
						erro_log(log, msglog);
					}
					f=f->prox;
				}
			}

			fscanf(arq,"%d", (setMov+2));
			if(*(setMov+2)<0)
				erro_log(log, "Valor invalido passado no iniciasimulacao");
			print_log(log, "set iniciasimulacao com sucesso!");

			//INICIA SIMULAÇÃO 

			srand(time(NULL)); //Randomiza de acordo com o horario do PC

			Virus *setVirus;
			int aux, agente=0;

			for(int i=0; i<*(setMov+2); i++){

				if(headVirus.qtd)	// Evita erro Exceção de ponto flutuante
					aux=rand() % headVirus.qtd;
				else
					aux=0;

				setVirus=headVirus.primeiro;
				
				for(int j=0; j<aux; j++)	// RANDOM VIRUS
					setVirus=setVirus->prox;

				if(headVirus.qtd)	//Evita falha de segmentacao
					move_virus(log, setVirus);	

				agente++;

				if(setVirus != NULL)	//Evita erro de falha de segmentação
					if((setVirus->mov == *(setMov+1)) && (headVirus.primeiro != NULL)){	//VIRUS MULTIPLICA

						setVirus->mov=0;

						for(int k=0; k<2; k++){
					
							Virus *auxHead, *auxVirus=aloca_Virus(setVirus->target, headVirus.ultimo, NULL);
							if(auxVirus == NULL)
									erro_log(log, "O Sistema nao liberou memoria");
							auxHead=headVirus.ultimo;
							auxHead->prox=auxVirus;
							headVirus.ultimo=auxVirus;
							headVirus.qtd++;
						}
					
						char msglog[39]="[+] Virus se multilplicou na familia ";
						Familia *aux=setVirus->target;
						strcat(msglog, aux->nome);
						print_log(log, msglog);
					}//	END MULTIPLICAÇÃO

				if(agente == *(setMov)){ //AGENTE ATUA

					agente=0;

					aux=rand() % headFamilia.qtd;

					Familia *setFamilia = headFamilia.primeiro;

					for(int k=0; k<aux; k++)	//RANDOM FAMILIA
						setFamilia=setFamilia->prox;

					char msglog[25]="[*] Agente set familia ";
					strcat(msglog, setFamilia->nome);
					print_log(log, msglog);
						
					if(headVirus.qtd){
						int vrf=agente_atua(log, setFamilia, &headVirus);
						//VRF virifica os erros do Agente;
						(vrf>0) ? acertos++ : erros++;

						if(setFamilia->doente) // entra c estiver doente ;
							agente_saude(log, setFamilia);
					}

					if(headVirus.primeiro == NULL){
						*(setMov+2)=(i+1);
						print_log(log, "[-] Todos os virus forom mortos");
						verifica_status(headFamilia.primeiro, status, mda80);
						*(status+1)=0;
						break;
					}
				}//END AGENTE ATUA
				verifica_status(headFamilia.primeiro, status, mda80);
			} //FIM DO FOR
		}// FIM DO IF
		if(!(strcmp(funcao,"verificasurto"))){

			log = fopen("log.txt", "a");

			fprintf(log, "\n\nNúmero de vírus inicias: %d\n", bkpVirus);
			fprintf(log, "Número de vírus finais: %d\n", headVirus.qtd);
			fprintf(log, "Número de movimentos totais dos vírus: %d\n", *(setMov+2));
			fprintf(log, "Número de erros dos agentes (não tinha vírus): %d\n", erros);
			fprintf(log, "Número de acertos dos agentes (matar vírus): %d\n", acertos);

			if(*(status)){
				if(*(status+1))
					fprintf(log, "Status da Epidemia: Estado de emergência\n");
				else
					fprintf(log, "Status da Epidemia: Houve surto mais foi controlado/erradicado\n");
			}else
				fprintf(log, "Status da Epidemia: Não houve surto\n");
			fclose(log);
		}
	}	//FIM WHILE

}	//END main//

void print_log(FILE *log, char *msg){

	struct tm *date;
	time_t segundos;

	log = fopen("log.txt", "a");
	time(&segundos);
	date = localtime(&segundos);
	fprintf(log, "[%d:%d:%d] %s\n", date->tm_hour, date->tm_min, date->tm_sec, msg);
	fclose(log);
}	//END PRINT_LOG

void erro_log(FILE *log, char *msg){

	struct tm *date;
	time_t segundos;

	printf("Erro!\n");
	log = fopen("log.txt", "w");
	time(&segundos);
	date = localtime(&segundos);
	fprintf(log, "[%d:%d:%d] Error: %s\n", date->tm_hour, date->tm_min, date->tm_sec, msg);
	fclose(log);
	exit(1); //Exit vale 1 pq deu erro;
}	//END ERRO_LOG

Familia *insereFamilia(char *n, int qtd){

	Familia *aux;

	aux = (Familia *) malloc(sizeof(Familia));

	if(aux == NULL)	return NULL;
	
	strcpy(aux->nome, n);
	aux->pessoas=qtd;
	aux->doente=0;
	aux->prox=NULL;
	aux->list=NULL;
	aux->nFamilias=0;

	return aux;
}	//END INSEREFAMILIA

Familia *busca(Familia *no, char *str){

	while(no != NULL){
		if(strcmp(str, no->nome) == 0 ){			
			return no;
		}
		no=no->prox;
	}
	return NULL;
}	//END BUSCA

int ligafamilias(Familia *f1, Familia *f2){

	Ligacao *aux = (Ligacao *) malloc(sizeof(Ligacao));
	if(aux == NULL)	return 0;
	
	aux->familia=f2;
	aux->prox=NULL;

	Ligacao *verifcador = f1->list;

	if(f1->list == NULL){
		f1->list = aux;
		f1->nFamilias++;
	}else{
		while(verifcador != NULL){
			
			if(verifcador->familia == f2){
				return 1; // JA EXISTE ESSA FAMILIA NA LISTA;
			}
			if(verifcador->prox == NULL)	break; //Somente para verificar c ja existe na lista; 
			verifcador=verifcador->prox;
			
		}
		verifcador->prox=aux;
		f1->nFamilias++;
	}//FIM F1

	aux = (Ligacao *) malloc(sizeof(Ligacao));
	if(aux == NULL)	return 0;

	aux->familia=f1;
	aux->prox=NULL;

	verifcador = f2->list;

	if(f2->list == NULL){
		f2->list = aux;
		f2->nFamilias++;
	}else{
		while(verifcador != NULL){
			
			if(verifcador->familia == f1){
				return 1; // JA EXISTE ESSA FAMILIA NA LISTA;
			}
			if(verifcador->prox == NULL)	break; //Somente para verificar c ja existe na lista; 
			verifcador=verifcador->prox;
		}
		verifcador->prox=aux;
		f2->nFamilias++;
	}

	return 2;
}	// END LIGA FAMILIA


Virus *aloca_Virus(Familia *alvo, Virus *back, Virus *next){

	Virus *aux = (Virus *) malloc(sizeof(Virus));

	if(aux == NULL){
		printf("Nao liberou espaco\n");
		return NULL;
	}
	aux->target=alvo;
	alvo->doente=-1;
	aux->mov=0;
	aux->ante=back;
	aux->prox=next;

	return aux;
}	//END ALOCAVIRUS

void move_virus(FILE *log, Virus *virus){

	Familia *f = virus->target;

	//para verificar c a familia tem ligação com alguem

	char msglog[42]="[ ] O virus se moveu da familia "; //28+2+6+2
	strcat(msglog, f->nome);
	strcat(msglog, " para ");

	Ligacao *listaF = f->list;

	int nRand = rand() % f->nFamilias;

	for(int i=0; i<nRand; i++)
		listaF=listaF->prox;

	virus->target = listaF->familia; //set nova familia pro virus

	virus->mov++;
	f = virus->target;	//Mostra pra onde foi o virus
	f->doente = -1;

	strcat(msglog, f->nome);
	print_log(log, msglog);

}	//END MOVE VIRUS

int agente_atua(FILE *log, Familia *f, HeadV *head){

	Virus *virus = (*head).primeiro, *aux;
	int vrf, cont=0;	//vrf verifica c existe virus pra evitar erros
	char msglog[43]="[-] 0 Agente removeu um virus da familia ";
	strcat(msglog, f->nome);

	//printf("AGENTE ATUA++++\n");

	while(virus != NULL){
		vrf=0;
		if(virus->target == f){	//Achou o virus que esta na familia
			print_log(log, msglog);
			(*head).qtd--;	//SUbtrai quantia do no cabeça
			cont++;

			if((virus->ante == NULL) && (virus->prox == NULL)){
				free(virus);
				(*head).primeiro = NULL;
				(*head).qtd=0;
				break;
			}else if(virus->ante == NULL){
				vrf++;
				aux=virus;
				virus=virus->prox;
				virus->ante=NULL;
				(*head).primeiro=virus;
			}else if(virus->prox == NULL){
				aux=virus->ante;
				aux->prox=NULL;
				(*head).ultimo=aux;
				free(virus);
				break;
			}else{
				aux=virus->ante;
				aux->prox=virus->prox;
				aux=aux->prox;
				aux->ante=virus->ante;
				free(virus);
			}
		}
		if(vrf)
			free(aux);
		else
			virus=virus->prox;
	}// END WHILE
	return cont;
}	//END AGENTE ATUA

void agente_saude(FILE *log, Familia *f){

	f->doente=0;

	char msglog[27]="[-] A familia ";
	strcat(msglog, f->nome);
	strcat(msglog, " foi curada");
	print_log(log, msglog);

}	//END AGENTE SAUDE

void verifica_status(Familia *f, unsigned short *v, int media){

	int aux=0;

	while(f != NULL){
		if(f->doente)
			aux+=f->pessoas;;
		f=f->prox;
	}

	if(aux>media){ //Ha surto
		*(v)=1;	
		*(v+1)=1;
	}else// nao ha
		*(v+1)=0;
}