#ifndef CPU_H
#define CPU_H

#include <string.h>
#include "Registrador.h"
#include "Instrucao.h"
#include "Pilha.h"

class CPU {
private:
	char BUFFER[200];
public:
	TabelaRegistrador *Registradores_Geral; // AX BX CX,...
	TabelaRegistrador *Registradores_Segmento;// CS, DS, ES, SS
	TabelaRegistrador *Registrador_Flags; // CF PF AF ZF SF TF IF DF OF
	TabelaRegistrador *IP;
	TabelaInstrucao *Instrucoes;
	char DATA[16];
	char ADDRESS[16];
	int Bus_Int =0;
	char BIU[16] = { '0','0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0' };
	int ULA_T1 = 0;
	int ULA_T2 = 0;
	int ULA_OUT = 0;
	Pilha *QUEUE = CriaPilha(6);//salva o valor em decimal e não em binario
	Pilha *PP = CriaPilha(16);
#pragma region ULA
	void ADD() {
		ULA_OUT = ULA_T1 + ULA_T2;
	}
	void SUB() {
		ULA_OUT = ULA_T1 - ULA_T2;
	}
	void MUL() {
		ULA_OUT = ULA_T1 * ULA_T2;
	}
	void DIV() {
		ULA_OUT = ULA_T1 / ULA_T2;
	}
	void INC() {
		ULA_OUT = ULA_T1 + 1;
	}
	void DEC() {
		ULA_OUT = ULA_T1 - 1;
	}
#pragma endregion

#pragma region DATA_TRANSFER
	void MOV(Registrador *Origem, Registrador *Destino) {//Regitrador Registrador
		Bus_Int = Registrador_RecebeDado(Origem);

		Registrador_AdicionaDado(Destino, Bus_Int);
	}
	void MOV(Registrador *Origem, char *Destino) {// Registrador Memoria
		Bus_Int = Registrador_RecebeDado(Origem);
//		Registrador_AdicionaDado(Destino, Bus_Int);
	}
	void MOV(char *Origem, Registrador *Destino) {// Memoria Registrdor
///		Bus_Int = Registrador_RecebeDado(D);
		//		Registrador_AdicionaDado(Destino, Bus_Int);
	}
	void MOV(Registrador *Origem, int Destino) {// Registrador valor
		Bus_Int = Registrador_RecebeDado(Origem);
		//		Registrador_AdicionaDado(Destino, Bus_Int);
	}
	void PUSH(char *Memoria) {
	}
	void PUSH(Registrador *dado) {

	}
	void POP(char *Memoria) {
	}
	void POP(Registrador *dado) {

	}
#pragma endregion

};



extern CPU i8086;
#endif