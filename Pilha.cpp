#include <stdlib.h>
#include "Pilha.h"

bool Cheio(Pilha *P) {
	if (P->Topo == P->Max - 1) {
		return true;
	}
	else {
		return false;
	}
}


bool Vazia(Pilha *P) {
	if (P->Topo == -1) {
		return true;
	}
	else {
		return false;
	}
}

void Empilha(Pilha *P, int Dado) {
	if (Cheio(P) == false) {
		P->Topo++;
		P->Dados[P->Topo] = Dado;
	}
}

void Desempilha(Pilha *P) {
	if (Vazia(P) == false) {
		P->Dados[P->Topo] = 0;
		P->Topo--;
	}
}

Pilha *CriaPilha(int Tamanho) {
	Pilha *NovaPilha = 0;
	if (Tamanho > 0) {
		NovaPilha = (Pilha*)malloc(sizeof(Pilha));
		NovaPilha->Topo = -1;
		NovaPilha->Max = Tamanho;
		NovaPilha->Dados = (int*)malloc(NovaPilha->Max * sizeof(int));
	}
	return NovaPilha;
}