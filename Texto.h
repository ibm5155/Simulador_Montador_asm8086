#ifndef _TEXTO_HEADER
#define _TEXTO_HEADER

#include <stdio.h>
#include "enums.h"


struct Linha {
	char *Caracteres;
	int QntCaracteres;// 0 = vazio, 1 = 1 caracter,...
};

struct Texto {
	Linha *Linhas;
	int QntLinhas;// 0 = vazio, 1 = 1 linha,...
	char *Fonte;// local onde será armazenado o texto
	bool Salvo;//SIM/NAO
};

Texto *CarregaTexto(char *endereco);
void AdicionaLinha(Texto * texto,char *adicionar);

void Destroi_Texto(Texto *t);

char  *CarregaPalavra(FILE *arquivo);
char *Carrega_Proxima_Palavra(Linha *linha, int *posicao);

char *InteiroParaCharBinario(int Decimal, bool Tamanho);
int CharBinarioParaInteiro(char *Binario);

char *GeraLixo(int Quantidade);
#endif