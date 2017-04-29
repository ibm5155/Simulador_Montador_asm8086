#ifndef Pilha_OBJ_H
#define Pilha_OBJ_H
struct PilhaOBJ {
	void *Ponteiro;
	int Tipo;
	bool RefMemoria;
};

struct TabelaPilhaOBJ {
	PilhaOBJ *pilhaobj;
	int Quantidade;
};

TabelaPilhaOBJ *CriaTabelaPonteiroOBJ();
/**
	Nota: Em caso de uma string numerica, o RefMemoria indica se o número é de 8 ou 16 bits com o (8_BITS 16_BITS)
	Tipo: Tipo de ponteiro (Registrador, Label ,... usar enum Tipo_Dado)
	Elemento: Ponteiro da estrutura que será lida no opcode
*/
void AdicionaTabela(TabelaPilhaOBJ *Tabela, void *Elemento, int Tipo, bool RefMemoria);
void LimpaTabela(TabelaPilhaOBJ *tabela);

#endif