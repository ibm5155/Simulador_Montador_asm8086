#ifndef _INSTRUCAO_HEADER
#define _INSTRUCAO_HEADER
#include "enums.h"

struct Instrucao {
	char Nome[5]; //MOV, MUL,ADD,...
	bool Opcode[8];
	char Operacao;// MEMORIA_REGISTRADOR,REGISTRADOR_MEMORIA,MEMORIA_MEMORIA,REGISTRADOR_REGISTRADOR,VAZIO

};

struct TabelaInstrucao{
	Instrucao *Instrucoes;
	int QntInstrucao;// 0 vazio, 1= 1,...
	bool TabelaValida;//SIM/NAO : Indica se a estrutura da tabela é integra (ela foi carregada sem erros)

};

TabelaInstrucao *CriaTabelaInstrucao();
void LimpaTabela(TabelaInstrucao *tabela);
void AdicionaTabela(TabelaInstrucao * tabela, char * Nome, bool * Opcode, char Operacao);
#endif