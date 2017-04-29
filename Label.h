#ifndef _LABEL_STRUCT
#define _LABEL_STRUCT

struct Label {
	char *Nome;
	char *ArquivoFonte;
	int Linha;// que � referenciado.
	bool Opcode[16];//tamanho maximo de uma label = 65535  (bate com o limite de mem�ria)
};

struct TabelaLabel {
	Label *label;
	int quantidade;//0 = vazio, 1 = 1 ,...
	bool TabelaValida;//SIM/NAO : Indica se a estrutura da tabela � integra (ela foi carregada sem erros)
};

TabelaLabel *CriaTabelaLabel();
void LimpaTabela(TabelaLabel *tabela);
void AdicionaTabela(TabelaLabel *tabela, char *Nome, char *ArquivoFonte, int Linha);
void AdicionaOpcode(TabelaLabel *tabelalabel, char *NomeLabel, int Opcode);
Label *ProcuraLabel(TabelaLabel *tabelalabel, char *Nome);
#endif