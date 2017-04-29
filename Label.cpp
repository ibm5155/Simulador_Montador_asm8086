#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Label.h"

TabelaLabel * CriaTabelaLabel()
{
	TabelaLabel *tabela = 0;
	tabela = (TabelaLabel*)malloc(sizeof(TabelaLabel));
	tabela->quantidade = 0;
	tabela->label = 0;
	return tabela;
}

void LimpaTabela(TabelaLabel * tabela)
{
	int c = 0;
	if (tabela) {
		for (c = tabela->quantidade -1; c >=0; c--) {
			if (tabela->label[c].ArquivoFonte) {
				free(tabela->label[c].ArquivoFonte);
			}if (tabela->label[c].Nome) {
				free(tabela->label[c].Nome);
			}
		}
		free(tabela->label);
		free(tabela);
		tabela = 0;
	}
}

void AdicionaTabela(TabelaLabel * tabela, char * Nome, char * ArquivoFonte, int Linha)
{
	int i = 0;
	if (tabela->quantidade) {
		tabela->label = (Label*)realloc(tabela->label, (tabela->quantidade + 1) * sizeof(Label));
	}
	else {
		tabela->label = (Label*)malloc((tabela->quantidade + 1) * sizeof(Label));
	}
	tabela->label[tabela->quantidade].Linha = Linha;
	tabela->label[tabela->quantidade].Nome = 0; 
	tabela->label[tabela->quantidade].ArquivoFonte = 0;
	if (Nome) { //caso não tenha cido informado o ponteiro da string vai continuar nulo
		i = strlen(Nome);
		tabela->label[tabela->quantidade].Nome = (char*)malloc((i + 2) * sizeof(char));
		strcpy(tabela->label[tabela->quantidade].Nome, Nome);
		tabela->label[tabela->quantidade].Nome[i + 1] = '\0';
	}if (ArquivoFonte) {
		i = strlen(ArquivoFonte);
		tabela->label[tabela->quantidade].ArquivoFonte = (char*)malloc((i + 2) * sizeof(char));
		strcpy(tabela->label[tabela->quantidade].ArquivoFonte, ArquivoFonte);
		tabela->label[tabela->quantidade].ArquivoFonte[i + 1] = '\0';
	}
	tabela->quantidade++;
}


void AdicionaOpcode(TabelaLabel *tabelalabel, char *NomeLabel, int Opcode) {
//baseado no código http://stackoverflow.com/questions/14104208/convert-integer-to-binary-and-store-it-in-an-integer-array-of-specified-sizec
	int strlenght = strlen(NomeLabel);
	int i = 0;
	int c = 0;
	//faz uma copia do NomeLabel
	char *string = (char*)malloc((strlenght + 2) * sizeof(char));
	strcpy(string, NomeLabel);
	string[strlenght] = '\0';

	Opcode = Opcode / 8; // converte de bits para bytes

	//procura o ponteiro da label
	while (c < tabelalabel->quantidade) {
		if (_stricmp(string, tabelalabel->label[c].Nome) == 0) {
			break;
		}
		c++;
	}

	//converte o valor decimal para binario
	for (i = 0; i < 16; ++i) {  // assuming a 32 bit int
		tabelalabel->label[c].Opcode[15-i] = Opcode & (1 << i) ? 1 : 0;
	}
	free(string);
}

/**
	Retorna zero se não encontrada.
	Retorna um ponteiro se encontrada.
*/
Label *ProcuraLabel(TabelaLabel *tabelalabel,char *Nome) {
	int c = 0;
	Label *Encontrada = 0;
	int strlenght = strlen(Nome);
	char *string = (char*)malloc((strlenght + 2) * sizeof(char));
	strcpy(string, Nome);
	string[strlenght] = '\0';

	//procura o ponteiro da label
	while (c < tabelalabel->quantidade) {
		if (_stricmp(string, tabelalabel->label[c].Nome) == 0) {
			Encontrada = &tabelalabel->label[c];
			c = tabelalabel->quantidade;
		}
		c++;
	}
	return Encontrada;
}