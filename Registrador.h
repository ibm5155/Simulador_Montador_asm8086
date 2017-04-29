#ifndef _REG_HEADER
#define _REG_HEADER

#include "enums.h"

struct Registrador {
	char Nome[3];
	bool Tamanho;// _8BITS, _16BITS
	bool FazReferenciaMemoria;// SIM NAO
	int Dado; //usado para _16BITS, _8BITS irá usar a metade
	bool Opcode[4];
	int High_offset;//caso seja um registrador de 16bits, referencia as partes de 8 bits aqui, Ex: AX : High = AH Low = AL
	int Low_offset;//caso seja um registrador de 16bits, referencia as partes de 8 bits aqui, Ex: AX : High = AH Low = AL
	//UPDATE: não da pra usar a estrutura Registrador *High, *Low pois geraria perda de referência durante o processo de carregamento ( o ponteiro não seria alterado enquanto a estrutura do registrador muda de posiçao na memória)
};

struct TabelaRegistrador {
	Registrador *Registradores;
	int Quantidade;
	bool TabelaValida;//SIM/NAO : Indica se a estrutura da tabela é integra (ela foi carregada sem erros)
};
/**
Entrada NULL indica que será carregado o arquivo de registradores padrão (Registrador.txt)
*/
TabelaRegistrador *CriaTabelaRegistrador(char *Endereco);
TabelaRegistrador * CriaTabelaRegistradorVazia();
void LimpaTabela(TabelaRegistrador *tabela);
void AdicionaTabela(TabelaRegistrador * tabela, char * Nome, bool Tamanho, bool FazReferenciaMemoria, char * High, char * Low, char *Opcode);
void Registrador_AdicionaDado(TabelaRegistrador *Tab,Registrador *Reg, int Dado);
int Registrador_RecebeDado(TabelaRegistrador *Tab, Registrador *Reg);

/**
Retorna a posição da tabela onde se encontra o registrador requisitado
Caso não encontrado retorna -1.
Caso a tabela seja invalida(nao existe), é retornado -2.
*/
int Registrador_PegaID(TabelaRegistrador *Tab, char *Nome);

/**
Retorna a posição da tabela onde se encontra o registrador requisitado
Caso não encontrado retorna -1.
Caso a tabela seja invalida(nao existe), é retornado -2.
*/
int Registrador_PegaID(TabelaRegistrador *Tab, char *Opcode, bool Por_Opcode);


#endif