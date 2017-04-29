#ifndef _REG_HEADER
#define _REG_HEADER

#include "enums.h"

struct Registrador {
	char Nome[3];
	bool Tamanho;// _8BITS, _16BITS
	bool FazReferenciaMemoria;// SIM NAO
	int Dado; //usado para _16BITS, _8BITS ir� usar a metade
	bool Opcode[4];
	int High_offset;//caso seja um registrador de 16bits, referencia as partes de 8 bits aqui, Ex: AX : High = AH Low = AL
	int Low_offset;//caso seja um registrador de 16bits, referencia as partes de 8 bits aqui, Ex: AX : High = AH Low = AL
	//UPDATE: n�o da pra usar a estrutura Registrador *High, *Low pois geraria perda de refer�ncia durante o processo de carregamento ( o ponteiro n�o seria alterado enquanto a estrutura do registrador muda de posi�ao na mem�ria)
};

struct TabelaRegistrador {
	Registrador *Registradores;
	int Quantidade;
	bool TabelaValida;//SIM/NAO : Indica se a estrutura da tabela � integra (ela foi carregada sem erros)
};
/**
Entrada NULL indica que ser� carregado o arquivo de registradores padr�o (Registrador.txt)
*/
TabelaRegistrador *CriaTabelaRegistrador(char *Endereco);
TabelaRegistrador * CriaTabelaRegistradorVazia();
void LimpaTabela(TabelaRegistrador *tabela);
void AdicionaTabela(TabelaRegistrador * tabela, char * Nome, bool Tamanho, bool FazReferenciaMemoria, char * High, char * Low, char *Opcode);
void Registrador_AdicionaDado(TabelaRegistrador *Tab,Registrador *Reg, int Dado);
int Registrador_RecebeDado(TabelaRegistrador *Tab, Registrador *Reg);

/**
Retorna a posi��o da tabela onde se encontra o registrador requisitado
Caso n�o encontrado retorna -1.
Caso a tabela seja invalida(nao existe), � retornado -2.
*/
int Registrador_PegaID(TabelaRegistrador *Tab, char *Nome);

/**
Retorna a posi��o da tabela onde se encontra o registrador requisitado
Caso n�o encontrado retorna -1.
Caso a tabela seja invalida(nao existe), � retornado -2.
*/
int Registrador_PegaID(TabelaRegistrador *Tab, char *Opcode, bool Por_Opcode);


#endif