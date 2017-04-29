#include "Variavel.h"
#include "Texto.h"
#include "Log.h"
#include "Mmain.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//CarregaPalavra

//CRIAR
void AdicionaTabela(TabelaVariavel * tabela, char * Nome, char *Fonte, int Linha, int Opcode, int Dado, bool Tamanho)
{
	//baseado no código http://stackoverflow.com/questions/14104208/convert-integer-to-binary-and-store-it-in-an-integer-array-of-specified-sizec
	int i = 0;
	if (tabela->Quantidade > 0) {
		tabela->Variaveis = (Variavel*)realloc(tabela->Variaveis, (tabela->Quantidade + 1) * sizeof(Variavel));
	}else {
		tabela->Variaveis= (Variavel*)malloc(sizeof(Variavel));
	}
	i = strlen(Nome);
	tabela->Variaveis[tabela->Quantidade].Nome = (char*)malloc((i + 2) * sizeof(char));
	strcpy(tabela->Variaveis[tabela->Quantidade].Nome, Nome);
	tabela->Variaveis[tabela->Quantidade].Nome[i + 1] = '\0';

	i = strlen(Fonte);
	tabela->Variaveis[tabela->Quantidade].ArquivoFonte = (char*)malloc((i + 2) * sizeof(char));
	strcpy(tabela->Variaveis[tabela->Quantidade].ArquivoFonte, Fonte);
	tabela->Variaveis[tabela->Quantidade].ArquivoFonte[i + 1] = '\0';

	tabela->Variaveis[tabela->Quantidade].Linha = Linha;

	tabela->Variaveis[tabela->Quantidade].Dado = Dado;

	tabela->Variaveis[tabela->Quantidade].Tamanho = Tamanho;
	i = 0;

	Opcode = Opcode / 8; // converte de bits para bytes

	for (i = 0; i < 16; ++i) {  // assuming a 32 bit int
		tabela->Variaveis[tabela->Quantidade].Opcode[15 - i] = Opcode & (1 << i) ? 1 : 0;
	}
	tabela->Quantidade++;
}

/**
//retorno: um ponteiro de uma tabela de variavel
*/
TabelaVariavel * CriaTabelaVariavel() {
	FILE *f = 0;
	char *nome = 0;
	char *opcode = 0;
	char *operacao = 0;
	char Erro[100];
	int linha = 1;
	bool opcodeBIN[8];
	bool opcodeValido = false;
	int i = 0;
	TabelaVariavel *tabela = 0;

	tabela = (TabelaVariavel*)malloc(sizeof(TabelaVariavel));
	tabela->Quantidade = 0;
	tabela->TabelaValida = SIM;
	tabela->Variaveis = 0;
	return tabela;
}

void LimpaTabela(TabelaVariavel * tabela)
{
	int c = 0;
	if (tabela) {
		for (c = tabela->Quantidade -1; c >= 0; c--) {
			if (tabela->Variaveis[c].ArquivoFonte) {
				free(tabela->Variaveis[c].ArquivoFonte);
			}if (tabela->Variaveis[c].Nome) {
				free(tabela->Variaveis[c].Nome);
			}
		}
		free(tabela->Variaveis);
		free(tabela);
		tabela = 0;
	}
}

void AdicionaOpcode(TabelaVariavel  *tabela, int *Opcode) {
	//baseado no código http://stackoverflow.com/questions/14104208/convert-integer-to-binary-and-store-it-in-an-integer-array-of-specified-sizec
	int i = 0;
	int c = 0;
	int fim = 16;
	//faz uma copia da Variavel

	Opcode[0] = Opcode[0] / 8; // converte de bits para bytes

	//procura o ponteiro da Variavel
	while (c < tabela->Quantidade) {
		if (tabela->Variaveis[c].Tamanho == _16BITS) {
			fim = 16;
		}
		else {
			fim = 8;
		}
		//converte o valor decimal para binario
		for (int i = 0; i < fim; i++) {
			tabela->Variaveis[c].Opcode[fim - 1 - i] = 0;
		}
		for (i = 0; i < fim; ++i) {  // assuming a 32 bit int
			tabela->Variaveis[c].Opcode[fim - 1 - i] = Opcode[0] & (1 << i) ? 1 : 0;
		}
		Opcode[0] += fim;
		c++;
	}


}