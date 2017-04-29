#include "Instrucao.h"
#include "Texto.h"
#include "Log.h"
#include "Mmain.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//CarregaPalavra

//CRIAR
void AdicionaTabela(TabelaInstrucao * tabela,char * Nome, bool * Opcode, char Operacao)
{
	int i = 0;
	if (tabela->QntInstrucao > 0) {
		tabela->Instrucoes = (Instrucao*)realloc(tabela->Instrucoes,(tabela->QntInstrucao+1)*sizeof(Instrucao));
		tabela->Instrucoes[tabela->QntInstrucao].Operacao = Operacao;
		i = strlen(Nome);
		//nome de tamanho fixo
		strcpy(tabela->Instrucoes[tabela->QntInstrucao].Nome, Nome);
		tabela->Instrucoes[tabela->QntInstrucao].Nome[i + 1] = '\0';
		i = 0;
		while (i < 8) {
			tabela->Instrucoes[tabela->QntInstrucao].Opcode[i] = Opcode[i];
			i++;
		}
		tabela->QntInstrucao++;
	}else {
		tabela->Instrucoes = (Instrucao*)malloc(sizeof(Instrucao));
		tabela->Instrucoes[tabela->QntInstrucao].Operacao = Operacao;
		i = strlen(Nome);
		//nome de tamanho fixo
		strcpy(tabela->Instrucoes[tabela->QntInstrucao].Nome, Nome);
		tabela->Instrucoes[tabela->QntInstrucao].Nome[i + 1] = '\0';
		i = 0;
		while (i < 8) {
			tabela->Instrucoes[tabela->QntInstrucao].Opcode[i] = Opcode[i];
			i++;
		}
		tabela->QntInstrucao++;
	}
}


//TESTAR
TabelaInstrucao * CriaTabelaInstrucao(){// retorno: 0 se não foi encontrado o arquivo de instruções ou outro número caso tenha criado uma tabela de instruções...
	FILE *f = 0;
	char *nome=0;
	char *opcode=0;
	char *operacao=0;
	char Erro[100];
	int linha = 1;
	bool opcodeBIN[8];
	bool opcodeValido = false;
	int i = 0;
	TabelaInstrucao *tabela = 0;

	f = fopen("Instrucao.txt", "r");
	if (f) {
		//só é criado a tabela se tiver certeza de ter pelo menos um arquivo aberto...
		tabela = (TabelaInstrucao*)malloc(sizeof(TabelaInstrucao));
		tabela->QntInstrucao = 0;
		tabela->Instrucoes = 0;
		tabela->TabelaValida = SIM;

		while (!feof(f)) {
			//todos os elementos devem estar bem definidos;
			//é checado elemento por elemento para ver se em alguma parte está faltando alguma informação
			nome = CarregaPalavra(f);
			if (nome == 0) {
				if (!feof(f)) {
					sprintf(Erro, "Erro instrucao.txt:nome da instrucao nao encontrado");
					AdicionaLog(Erro);
					AtualizaTelaMontador();
					tabela->TabelaValida = NAO;
				}
			}
			else {
				opcode = CarregaPalavra(f);
				if (opcode == 0) {
					sprintf(Erro, "Erro instrucao.txt:opcode nao encontrado da instrucao %s",nome);
					AdicionaLog(Erro);
					AtualizaTelaMontador();
					tabela->TabelaValida = NAO;
				}
				else {
					operacao = CarregaPalavra(f);
					if (operacao == 0) {
						sprintf(Erro, "Erro instrucao.txt:operacao nao encontrada de %s", nome);
						AdicionaLog(Erro);
						AtualizaTelaMontador();
						tabela->TabelaValida = NAO;
					}
				}
			}
			if (nome && opcode && operacao) {//se todos eles foram carregados
				//Transforma opcode de string para binario
				i = 0;
				opcodeValido = true;
				if (strlen(opcode) != 8) {//verificar se é 9, 8 ou 7
					sprintf(Erro, "Erro instrucao.txt, opcode de tamanho invalido %s de %s", opcode,nome);
					AdicionaLog(Erro);
					AtualizaTelaMontador();
					tabela->TabelaValida = NAO;
				}
				while (i < 8) {
					if (opcode[i] != '0' && opcode[i] != '1') {//se não for nem zero nem um então aborte
						sprintf(Erro, "Erro instrucao.txt, opcode invalido %s ",opcode);
						AdicionaLog(Erro);
						AtualizaTelaMontador();
						tabela->TabelaValida = NAO;
						break;
					}
					else {
						opcodeBIN[i] = (bool)(opcode[i] - '0'); //já é garantido no if acima que só temos char to tipos '1' ou '0'
					}
					i++;
				}
				//se tudo ocorrer corretamente, carregue a instrucao
				if (opcodeValido) {
					AdicionaTabela(tabela, nome, opcodeBIN, atoi(operacao));
				}
				else {
					tabela->TabelaValida = NAO;
				}
			}
			linha++;
		}
		fclose(f);
	}
	else {
		sprintf(Erro, "Erro arquivo que contem as instrucoes (instrucao.txt) nao foi encontrado");
		AdicionaLog(Erro);
		AtualizaTelaMontador();
		tabela->TabelaValida = NAO;
	}
	return tabela;
}

void LimpaTabela(TabelaInstrucao * tabela)
{
	int c = 0;
	if (tabela) {
		free(tabela->Instrucoes);
		free(tabela);
		tabela = 0;
	}
}

