#include <stdlib.h>
#include "PilhaObjetos.h"
#include "enums.h"

TabelaPilhaOBJ *CriaTabelaPonteiroOBJ() {
	TabelaPilhaOBJ *T = (TabelaPilhaOBJ*)malloc(sizeof(TabelaPilhaOBJ));
	T->Quantidade = 0;
	T->pilhaobj = 0;
	return T;
}

void AdicionaTabela(TabelaPilhaOBJ *Tabela, void *Elemento, int Tipo, bool RefMemoria) {
	int i = 0;
	if (Tabela->Quantidade) {
		Tabela->pilhaobj = (PilhaOBJ*)realloc(Tabela->pilhaobj, (Tabela->Quantidade + 2) * sizeof(PilhaOBJ));
	}
	else {
		Tabela->pilhaobj = (PilhaOBJ*)malloc((Tabela->Quantidade + 2) * sizeof(PilhaOBJ));
	}
	Tabela->pilhaobj[Tabela->Quantidade].Tipo = Tipo;
	Tabela->pilhaobj[Tabela->Quantidade].Ponteiro = Elemento;
	Tabela->pilhaobj[Tabela->Quantidade].RefMemoria = RefMemoria;

	Tabela->Quantidade++;
}

void LimpaTabela (TabelaPilhaOBJ *tabela) {
	int i = 0;
	if (tabela) {
		if (tabela->Quantidade) {
			while (i < tabela->Quantidade) {
				if (tabela->pilhaobj[i].Tipo == NUMERO) {
					free(tabela->pilhaobj[i].Ponteiro);
				}
				i++;
			}
			free(tabela->pilhaobj);
		}
		free(tabela);
	}
}