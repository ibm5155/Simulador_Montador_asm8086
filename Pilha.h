#ifndef _PILHA_HEADER
#define _PILHA_HEADER
typedef struct Pilha {
	int Topo;// vazio = -1
	int Max;// tamanho maximo, do ponto de vista humano
	int *Dados;//vetor
};

void Empilha(Pilha *P, int Dado);

void Desempilha(Pilha *P);

bool Cheio(Pilha *P);


bool Vazia(Pilha *P);

Pilha *CriaPilha(int Tamanho);

#endif