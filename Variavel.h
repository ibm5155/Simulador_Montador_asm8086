#ifndef _VAR_H
#define _VAR_H

struct Variavel {
	char *Nome;
	char *ArquivoFonte;
	int Linha;// que é referenciado.
	bool Opcode[16];
	int Dado;
	bool Tamanho; //_8bits _16bits
};

struct TabelaVariavel {
	Variavel *Variaveis;
	int Quantidade;// 0 vazio, 1= 1,...
	bool TabelaValida;//SIM/NAO : Indica se a estrutura da tabela é integra (ela foi carregada sem erros)

};

TabelaVariavel *CriaTabelaVariavel();
void LimpaTabela(TabelaVariavel *tabela);
void AdicionaTabela(TabelaVariavel * tabela, char * Nome, char *Fonte, int Linha, int Opcode, int Dado, bool Tamanho);
/**
	Adiciona o opcode a todas as variaveis da tabela
*/
void AdicionaOpcode(TabelaVariavel  *tabela, int *Opcode);
#endif