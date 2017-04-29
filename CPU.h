#ifndef CPU_H
#define CPU_H


#include <string.h>
#include "Registrador.h"
#include "Instrucao.h"
#include "Pilha.h"
#include "Dump_Simulacao.h"
#include "RAM.h"
#include "Texto.h"
#include "Cache.h"
#include <MT2D/MessageBox/MT2D_MessageBox.h>

#define cpu_memoria 0
#define cpu_biu 1
#define cpu_queue 2
#define cpu_decode 3
#define cpu_ip 4
#define cpu_incrementa_ip 5
#define cpu_registrador 6
#define cpu_ula 7

#define ula_t1 1
#define ula_t2 2
#define ula_t3 3

#define sizeofCache 16

extern unsigned char Memoria[65535];// 1 byte por char -> 64KB ram


class CPU {
private:
	char BUFFER[200];
public:
	struct Cache {
		//Cache 8 Bits pra evitar treta!!!
		int EndMemoria[sizeofCache] = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };
		unsigned char Memoria[sizeofCache] = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };
		int Hit = 0;
		int Miss = 0;

	};

	TabelaRegistrador *Registradores_Geral; // AX BX CX,...
	TabelaRegistrador *Registradores_Segmento;// CS, DS, ES, SS
	TabelaRegistrador *Registrador_Flags; // CF PF AF ZF SF TF IF DF OF
	TabelaRegistrador *IP;
	TabelaInstrucao *Instrucoes;
	Cache cache;
	char DATA[16];
	char ADDRESS[16];
	int Bus_Int = 0;
	int ULA_T1 = 0;
	int ULA_T2 = 0;
	int ULA_OUT = 0;
	int QUEUE[6] = { 0,0,0,0,0,0 };//salva o valor em decimal e não em binario
	int PILHA_PUSH_POP[12] = {0,0,0,0,0,0,0,0,0,0,0,0};
	int PILHA_PUSH_POP_ULTIMO = -1;// -1 = vazio

	int QUEUE_ULTIMO = 0;// -1 = vazio
	Pilha *PP = CriaPilha(16);

#pragma region CACHE
	bool Cache_Deu_Hit() {
		bool Hit = false;
		if (cache.EndMemoria[IP->Registradores[0].Dado % sizeofCache] == IP->Registradores[0].Dado)
		{
			Hit = true;
		}
		return Hit;
	}

	/**
		Dado = 16bits e não 8bits
	*/
	void Cache_Salva_Dado(int Dado, int Posicao, int Bits)
	{
		if (Bits == _8BITS)
		{
			cache.EndMemoria[Posicao % sizeofCache] = Posicao;
			cache.Memoria[Posicao % sizeofCache] = Dado;
		}
		else // 16 bits
		{
			unsigned char PT2 = Dado - ((Dado >> 8) << 8);
			cache.EndMemoria[Posicao % sizeofCache] = Posicao;
			cache.Memoria[Posicao % sizeofCache] = Dado >> 8; //pega os 8 bits
			cache.EndMemoria[(Posicao + 1) % sizeofCache] = Posicao + 1;
			cache.Memoria[(Posicao + 1) % sizeofCache] = PT2; //pega o restante

		}
	}
#pragma endregion LOGICA


#pragma region ULA
	void ADD(Registrador *Destino, Registrador *Origem) {
		Registrador_Flags->Registradores[4].Dado =3;
		sprintf(BUFFER, "ADD %s %s", Destino->Nome, Origem->Nome);
		Bus_Int = Registrador_RecebeDado(Registradores_Geral, Destino);
		ULA_T1 = Bus_Int;
		Dump_Simulacao(cpu_registrador, cpu_ula, Dump_Simulacao_registrador_indice(Destino->Nome), ula_t1, "Operacao pt1-4", BUFFER);
		Bus_Int = Registrador_RecebeDado(Registradores_Geral, Origem);
		ULA_T2 = Bus_Int;
		Dump_Simulacao(cpu_registrador, cpu_ula, Dump_Simulacao_registrador_indice(Origem->Nome), ula_t2, "Operacao pt2-4", BUFFER);
		Dump_Simulacao(cpu_queue, cpu_ula, 0, ula_t3, "Operacao pt3-4", BUFFER);
		ULA_OUT = ULA_T1 + ULA_T2;
		Bus_Int = ULA_OUT;
		Registrador_AdicionaDado(Registradores_Geral, Destino, Bus_Int);
		Dump_Simulacao(cpu_ula, cpu_registrador, Dump_Simulacao_registrador_indice(Destino->Nome), 0, "Operacao pt4-4", BUFFER);
	}

	void ADD(Registrador *Destino, int Origem, bool Origem_Direta) {
		sprintf(BUFFER, "ADD %s %d", Destino->Nome, Origem);
		Bus_Int = Registrador_RecebeDado(Registradores_Geral, Destino);
		ULA_T1 = Bus_Int;
		Dump_Simulacao(cpu_registrador, cpu_ula, Dump_Simulacao_registrador_indice(Destino->Nome), ula_t1, "Operacao pt1-4", BUFFER);
		Bus_Int = Origem;
		ULA_T2 = Bus_Int;
		Dump_Simulacao(cpu_queue, cpu_ula, 0, ula_t2, "Operacao pt2-4", BUFFER);
		Dump_Simulacao(cpu_queue, cpu_ula, 0, ula_t3, "Operacao pt3-4", BUFFER);
		ULA_OUT = ULA_T1 + ULA_T2;
		Bus_Int = ULA_OUT;
		Registrador_AdicionaDado(Registradores_Geral, Destino, Bus_Int);
		Dump_Simulacao(cpu_ula, cpu_registrador, Dump_Simulacao_registrador_indice(Destino->Nome), 0, "Operacao pt4-4", BUFFER);
	}

	void ADD(int Destino, Registrador *Origem) {
		sprintf(BUFFER, "ADD [%d] %s", Destino, Origem->Nome);
		if (Origem->Tamanho == _16BITS) {
			Bus_Int = Memoria[Destino];
			Bus_Int = Bus_Int << 8; // move a parte do high para cima (PRECISA SER TESTADO!!!)
			Bus_Int += Memoria[Destino + 1];// adiciona a parte do low ao bus int
		}
		else {
			Bus_Int = Memoria[Destino];
		}
		Dump_Simulacao(cpu_memoria, cpu_biu, 0, 0, "Operacao pt1-6", BUFFER);
		ULA_T1 = Bus_Int;
		Dump_Simulacao(cpu_biu, cpu_ula, 0, ula_t1, "Operacao pt2-6", BUFFER);
		Bus_Int = Registrador_RecebeDado(Registradores_Geral, Origem);
		ULA_T2 = Bus_Int;
		Dump_Simulacao(cpu_registrador, cpu_ula, Dump_Simulacao_registrador_indice(Origem->Nome), ula_t2, "Operacao pt3-6", BUFFER);
		Dump_Simulacao(cpu_queue, cpu_ula, 0, ula_t3, "Operacao pt4-6", BUFFER);
		ULA_OUT = ULA_T1 + ULA_T2;
		Bus_Int = ULA_OUT;
		Dump_Simulacao(cpu_ula, cpu_biu, 0, 0, "Operacao pt5-6", BUFFER);
		Memoria[Destino] = Bus_Int >> 8; // isola o high
		Cache_Salva_Dado(Memoria[Destino], Destino, _8BITS);
		Memoria[Destino + 1] = Bus_Int - Memoria[Destino] << 8; // isola o low
		Cache_Salva_Dado(Memoria[Destino+ 1], Destino + 1, _8BITS);

		Dump_Simulacao(cpu_biu, cpu_memoria, 0, 0, "Operacao pt6-6", BUFFER);

	}

	void ADD(int Destino, int Imediato) {
		if (Destino < 16) {
			char *DestinoBinario = InteiroParaCharBinario(Destino, _8BITS);
			int Id = Registrador_PegaID(Registradores_Geral, &DestinoBinario[4], true);// 8 - 3 = 5 bits, dos quais 1 é o final que indica o fim da string, então com isso isolamos																					   //																					    o endereço do registrador que é 4 bits
			sprintf(BUFFER, "ADD [%s] %d", Registradores_Geral->Registradores[Id].Nome, Imediato);
			Dump_Simulacao(cpu_registrador, cpu_queue, Dump_Simulacao_registrador_indice(Registradores_Geral->Registradores[Id].Nome), 0, "Operacao pt1-X", BUFFER);
			Bus_Int = Registrador_RecebeDado(Registradores_Geral, &Registradores_Geral->Registradores[Id]);
			Dump_Simulacao(cpu_queue, cpu_decode, 0, 0, "Operacao pt2-X", BUFFER);
			ADD(Bus_Int, Imediato);
		}
		else {
			sprintf(BUFFER, "ADD [%d] %d", Memoria, Imediato);

			Bus_Int = Memoria[Destino];
			Bus_Int = Bus_Int << 8; // move a parte do high para cima (PRECISA SER TESTADO!!!)
			Bus_Int += Memoria[Destino + 1];// adiciona a parte do low ao bus int
			Dump_Simulacao(cpu_memoria, cpu_biu, 0, 0, "Operacao pt1-6", BUFFER);

			ULA_T1 = Bus_Int;
			Dump_Simulacao(cpu_biu, cpu_ula, 0, ula_t1, "Operacao pt2-6", BUFFER);
			Bus_Int = Imediato;
			ULA_T2 = Bus_Int;
			Dump_Simulacao(cpu_queue, cpu_ula, 0, ula_t2, "Operacao pt3-6", BUFFER);
			Dump_Simulacao(cpu_queue, cpu_ula, 0, ula_t3, "Operacao pt4-6", BUFFER);
			ULA_OUT = ULA_T1 + ULA_T2;
			Bus_Int = ULA_OUT;
			Dump_Simulacao(cpu_ula, cpu_biu, 0, 0, "Operacao pt5-6", BUFFER);
			Memoria[Destino] = Bus_Int >> 8; // isola o high
			Memoria[Destino + 1] = Bus_Int - Memoria[Destino] << 8;
			Cache_Salva_Dado(Memoria[Destino], Destino, _8BITS);
			Cache_Salva_Dado(Memoria[Destino+1], Destino+1, _8BITS);

			Dump_Simulacao(cpu_biu, cpu_memoria, 0, 0, "Operacao pt6-6", BUFFER);
		}

	}

	void ADD(Registrador *Destino, int Origem) {
		sprintf(BUFFER, "ADD %s [%d]", Destino->Nome, Origem);
		Bus_Int = Registrador_RecebeDado(Registradores_Geral, Destino);
		ULA_T1 = Bus_Int;
		Dump_Simulacao(cpu_registrador, cpu_ula, Dump_Simulacao_registrador_indice(Destino->Nome), ula_t1, "Operacao pt1-5", BUFFER);

		if (Destino->Tamanho == _16BITS) {
			Bus_Int = Memoria[Origem];
			Bus_Int = Bus_Int << 8; // move a parte do high para cima (PRECISA SER TESTADO!!!)
			Bus_Int += Memoria[Origem + 1];// adiciona a parte do low ao bus int
		}
		else {
			Bus_Int = Memoria[Origem];
		}
		Dump_Simulacao(cpu_memoria, cpu_biu, 0, 0, "Operacao pt2-5", BUFFER);
		ULA_T2 = Bus_Int;
		Dump_Simulacao(cpu_biu, cpu_ula, 0, ula_t2, "Operacao pt3-5", BUFFER);
		Dump_Simulacao(cpu_queue, cpu_ula, 0, ula_t3, "Operacao pt4-5", BUFFER);
		ULA_OUT = ULA_T1 + ULA_T2;
		Bus_Int = ULA_OUT;
		Registrador_AdicionaDado(Registradores_Geral, Destino, Bus_Int);
		Dump_Simulacao(cpu_ula, cpu_registrador, Dump_Simulacao_registrador_indice(Destino->Nome), 0, "Operacao pt5-5", BUFFER);
	}

	void SUB(Registrador *Destino, Registrador *Origem) {
		sprintf(BUFFER, "SUB %s %s", Destino->Nome, Origem->Nome);
		Bus_Int = Registrador_RecebeDado(Registradores_Geral, Destino);
		ULA_T1 = Bus_Int;
		Dump_Simulacao(cpu_registrador, cpu_ula, Dump_Simulacao_registrador_indice(Destino->Nome), ula_t1, "Operacao pt1-4", BUFFER);
		Bus_Int = Registrador_RecebeDado(Registradores_Geral, Origem);
		ULA_T2 = Bus_Int;
		Dump_Simulacao(cpu_registrador, cpu_ula, Dump_Simulacao_registrador_indice(Origem->Nome), ula_t2, "Operacao pt2-4", BUFFER);
		Dump_Simulacao(cpu_queue, cpu_ula, 0, ula_t3, "Operacao pt3-4", BUFFER);
		ULA_OUT = ULA_T1 - ULA_T2;
		Bus_Int = ULA_OUT;
		Registrador_AdicionaDado(Registradores_Geral, Destino, Bus_Int);
		Dump_Simulacao(cpu_ula, cpu_registrador, Dump_Simulacao_registrador_indice(Destino->Nome), 0, "Operacao pt4-4", BUFFER);
	}

	void SUB(Registrador *Destino, int Origem, bool Origem_Direta) {
		sprintf(BUFFER, "SUB  %s %d", Destino->Nome, Origem);
		Bus_Int = Registrador_RecebeDado(Registradores_Geral, Destino);
		ULA_T1 = Bus_Int;
		Dump_Simulacao(cpu_registrador, cpu_ula, Dump_Simulacao_registrador_indice(Destino->Nome), ula_t1, "Operacao pt1-4", BUFFER);
		Bus_Int = Origem;
		ULA_T2 = Bus_Int;
		Dump_Simulacao(cpu_queue, cpu_ula, 0, ula_t2, "Operacao pt2-4", BUFFER);
		Dump_Simulacao(cpu_queue, cpu_ula, 0, ula_t3, "Operacao pt3-4", BUFFER);
		ULA_OUT = ULA_T1 - ULA_T2;
		Bus_Int = ULA_OUT;
		Registrador_AdicionaDado(Registradores_Geral, Destino, Bus_Int);
		Dump_Simulacao(cpu_ula, cpu_registrador, Dump_Simulacao_registrador_indice(Destino->Nome), 0, "Operacao pt4-4", BUFFER);
	}

	void SUB(int Destino, Registrador *Origem) {
		sprintf(BUFFER, "SUB [%d] %s", Destino, Origem->Nome);
		if (Origem->Tamanho == _16BITS) {
			Bus_Int = Memoria[Destino];
			Bus_Int = Bus_Int << 8; // move a parte do high para cima (PRECISA SER TESTADO!!!)
			Bus_Int += Memoria[Destino + 1];// adiciona a parte do low ao bus int
		}
		else {
			Bus_Int = Memoria[Destino];
		}
		Dump_Simulacao(cpu_memoria, cpu_biu, 0, 0, "Operacao pt1-6", BUFFER);
		ULA_T1 = Bus_Int;
		Dump_Simulacao(cpu_biu, cpu_ula, 0, ula_t1, "Operacao pt2-6", BUFFER);
		Bus_Int = Registrador_RecebeDado(Registradores_Geral, Origem);
		ULA_T2 = Bus_Int;
		Dump_Simulacao(cpu_registrador, cpu_ula, Dump_Simulacao_registrador_indice(Origem->Nome), ula_t2, "Operacao pt3-6", BUFFER);
		Dump_Simulacao(cpu_queue, cpu_ula, 0, ula_t3, "Operacao pt4-6", BUFFER);
		ULA_OUT = ULA_T1 - ULA_T2;
		Bus_Int = ULA_OUT;
		Dump_Simulacao(cpu_ula, cpu_biu, 0, 0, "Operacao pt5-6", BUFFER);
		Memoria[Destino] = Bus_Int >> 8; // isola o high
		Memoria[Destino + 1] = Bus_Int - Memoria[Destino] << 8; // isola o low (PRECISA SER TESTADO!!!)
		Cache_Salva_Dado(Memoria[Destino], Destino, _8BITS);
		Cache_Salva_Dado(Memoria[Destino+1], Destino+1, _8BITS);

		Dump_Simulacao(cpu_biu, cpu_memoria, 0, 0, "Operacao pt6-6", BUFFER);

	}

	void SUB(Registrador *Destino, int Origem) {
		sprintf(BUFFER, "SUB %s [%d]", Destino->Nome, Origem);
		Bus_Int = Registrador_RecebeDado(Registradores_Geral, Destino);
		ULA_T1 = Bus_Int;
		Dump_Simulacao(cpu_registrador, cpu_ula, Dump_Simulacao_registrador_indice(Destino->Nome), ula_t1, "Operacao pt1-5", BUFFER);

		Dump_Simulacao(cpu_memoria, cpu_biu, 0, 0, "Operacao pt2-5", BUFFER);
		if (Destino->Tamanho == _16BITS) {
			Bus_Int = Memoria[Origem];
			Bus_Int = Bus_Int << 8; // move a parte do high para cima (PRECISA SER TESTADO!!!)
			Bus_Int += Memoria[Origem + 1];// adiciona a parte do low ao bus int
		}
		else {
			Bus_Int = Memoria[Origem];
		}
		ULA_T2 = Bus_Int;
		Dump_Simulacao(cpu_biu, cpu_ula, 0, ula_t2, "Operacao pt3-5", BUFFER);

		Dump_Simulacao(cpu_queue, cpu_ula, 0, ula_t3, "Operacao pt4-5", BUFFER);
		ULA_OUT = ULA_T1 - ULA_T2;
		Bus_Int = ULA_OUT;
		Registrador_AdicionaDado(Registradores_Geral, Destino, Bus_Int);
		Dump_Simulacao(cpu_ula, cpu_registrador, Dump_Simulacao_registrador_indice(Destino->Nome), 0, "Operacao pt5-5", BUFFER);
	}

	void SUB(int Destino, int Imediato) {
		if (Destino < 16) {
			char *DestinoBinario = InteiroParaCharBinario(Destino, _8BITS);
			int Id = Registrador_PegaID(Registradores_Geral, &DestinoBinario[4], true);// 8 - 3 = 5 bits, dos quais 1 é o final que indica o fim da string, então com isso isolamos																					   //																					    o endereço do registrador que é 4 bits
			sprintf(BUFFER, "SUB [%s] %d", Registradores_Geral->Registradores[Id].Nome, Imediato);
			Dump_Simulacao(cpu_registrador, cpu_queue, Dump_Simulacao_registrador_indice(Registradores_Geral->Registradores[Id].Nome), 0, "Operacao pt1-X", BUFFER);
			Bus_Int = Registrador_RecebeDado(Registradores_Geral, &Registradores_Geral->Registradores[Id]);
			Dump_Simulacao(cpu_queue, cpu_decode, 0, 0, "Operacao pt2-X", BUFFER);
			ADD(Bus_Int, Imediato);
		}
		else {
			sprintf(BUFFER, "SUB [%d] %d", Memoria, Imediato);
			Dump_Simulacao(cpu_memoria, cpu_biu, 0, 0, "Operacao pt1-6", BUFFER);

			Bus_Int = Memoria[Destino];
			Bus_Int = Bus_Int << 8; // move a parte do high para cima (PRECISA SER TESTADO!!!)
			Bus_Int += Memoria[Destino + 1];// adiciona a parte do low ao bus int

			ULA_T1 = Bus_Int;
			Dump_Simulacao(cpu_biu, cpu_ula, 0, ula_t1, "Operacao pt2-6", BUFFER);

			Bus_Int = Imediato;
			ULA_T2 = Bus_Int;
			Dump_Simulacao(cpu_queue, cpu_ula, 0, ula_t2, "Operacao pt3-6", BUFFER);
			Dump_Simulacao(cpu_queue, cpu_ula, 0, ula_t3, "Operacao pt4-6", BUFFER);
			ULA_OUT = ULA_T1 - ULA_T2;
			Bus_Int = ULA_OUT;
			Dump_Simulacao(cpu_ula, cpu_biu, 0, 0, "Operacao pt5-6", BUFFER);
			Memoria[Destino] = Bus_Int >> 8; // isola o high
			Memoria[Destino + 1] = Bus_Int - Memoria[Destino] << 8;
			Cache_Salva_Dado(Memoria[Destino], Destino, _8BITS);
			Cache_Salva_Dado(Memoria[Destino + 1], Destino + 1, _8BITS);
			Dump_Simulacao(cpu_biu, cpu_memoria, 0, 0, "Operacao pt6-6", BUFFER);
		}

	}

	void MUL(Registrador *Origem) {

		sprintf(BUFFER, "MUL %s", Origem->Nome);
		Dump_Simulacao(cpu_registrador, cpu_ula, Dump_Simulacao_registrador_indice("AX"), ula_t1, "Operacao pt1-4", BUFFER);
		Bus_Int = Registradores_Geral->Registradores[Registrador_PegaID(Registradores_Geral, "AX")].Dado;
		ULA_T1 = Bus_Int;

		Dump_Simulacao(cpu_registrador, cpu_ula, Dump_Simulacao_registrador_indice(Origem->Nome), ula_t2, "Operacao pt2-4", BUFFER);
		Bus_Int = Origem->Dado;
		ULA_T2 = Bus_Int;

		Dump_Simulacao(cpu_queue, cpu_ula, 0, ula_t3, "Operacao pt3-4", BUFFER);

		ULA_OUT = ULA_T1 * ULA_T2;


		if (Origem->Tamanho == _16BITS) {

			Bus_Int = ULA_OUT >> 16;
			Dump_Simulacao(cpu_ula, cpu_registrador, Dump_Simulacao_registrador_indice("DX"), 0, "Operacao pt4-5", BUFFER);
			Registrador_AdicionaDado(Registradores_Geral, &Registradores_Geral->Registradores[Registrador_PegaID(Registradores_Geral, "DX")], Bus_Int);

			Bus_Int = ULA_OUT - Bus_Int << 16;
			Dump_Simulacao(cpu_ula, cpu_registrador, Dump_Simulacao_registrador_indice("AX"), 0, "Operacao pt5-5", BUFFER);
			Registrador_AdicionaDado(Registradores_Geral, &Registradores_Geral->Registradores[Registrador_PegaID(Registradores_Geral, "AX")], Bus_Int);
		}
		else {

			Dump_Simulacao(cpu_ula, cpu_registrador, Dump_Simulacao_registrador_indice("AX"), 0, "Operacao pt4-4", BUFFER);
			Bus_Int = ULA_OUT;
			Registrador_AdicionaDado(Registradores_Geral, &Registradores_Geral->Registradores[Registrador_PegaID(Registradores_Geral, "AX")], Bus_Int);
		}
	}

	void MUL(int Imediato, bool imediato) {

		sprintf(BUFFER, "MUL %d", Imediato);
		Bus_Int = Registradores_Geral->Registradores[Registrador_PegaID(Registradores_Geral, "AX")].Dado;
		ULA_T1 = Bus_Int;
		Dump_Simulacao(cpu_queue, cpu_ula, 0, ula_t1, "Operacao pt1-5", BUFFER);

		Bus_Int = Imediato;
		ULA_T2 = Bus_Int;
		Dump_Simulacao(cpu_queue, cpu_ula, 0, ula_t2, "Operacao pt2-5", BUFFER);

		Dump_Simulacao(cpu_queue, cpu_ula, 0, ula_t3, "Operacao pt3-5", BUFFER);

		ULA_OUT = ULA_T1 * ULA_T2;

		Bus_Int = ULA_OUT >> 16;
		Registrador_AdicionaDado(Registradores_Geral, &Registradores_Geral->Registradores[Registrador_PegaID(Registradores_Geral, "DX")], Bus_Int);
		Dump_Simulacao(cpu_ula, cpu_registrador, Dump_Simulacao_registrador_indice("DX"), 0, "Operacao pt4-5", BUFFER);

		Bus_Int = ULA_OUT - Bus_Int << 16;
		Registrador_AdicionaDado(Registradores_Geral, &Registradores_Geral->Registradores[Registrador_PegaID(Registradores_Geral, "AX")], Bus_Int);
		Dump_Simulacao(cpu_ula, cpu_registrador, Dump_Simulacao_registrador_indice("AX"), 0, "Operacao pt5-5", BUFFER);
	}

	void MUL(int memoria) {


		if (memoria < 16) {
			char *DestinoBinario = InteiroParaCharBinario(memoria, _8BITS);
			int Id = Registrador_PegaID(Registradores_Geral, &DestinoBinario[4], true);// 8 - 3 = 5 bits, dos quais 1 é o final que indica o fim da string, então com isso isolamos
																					   //																					    o endereço do registrador que é 4 bits
			sprintf(BUFFER, "MUL [%s]", Registradores_Geral->Registradores[Id].Nome);
			Dump_Simulacao(cpu_registrador, cpu_queue, Dump_Simulacao_registrador_indice(Registradores_Geral->Registradores[Id].Nome), 0, "Operacao pt1-x", BUFFER);
			Bus_Int = Registrador_RecebeDado(Registradores_Geral, &Registradores_Geral->Registradores[Id]);
			Dump_Simulacao(cpu_queue, cpu_decode, 0, 0, "Operacao pt2-x", BUFFER);
			Dump_Simulacao(cpu_incrementa_ip, cpu_ip, 0, 0, "Operacao pt3-x", BUFFER);
			MUL(Bus_Int);
		}
		else {
			sprintf(BUFFER, "MUL %d", memoria);

			/*Não sei o que tem que ser mostrado D:*/
			Dump_Simulacao(cpu_memoria, cpu_biu, 0, 0, "Operacao pt1-6", BUFFER);
			Bus_Int = Memoria[memoria] << 8;
			Bus_Int += Memoria[memoria + 1];
			ULA_T2 = Bus_Int;
			Dump_Simulacao(cpu_biu, cpu_ula, 0, ula_t2, "Operacao pt2-6", BUFFER);

			Bus_Int = Registradores_Geral->Registradores[Registrador_PegaID(Registradores_Geral, "AX")].Dado;
			ULA_T1 = Bus_Int;
			Dump_Simulacao(cpu_queue, cpu_ula, 0, ula_t1, "Operacao pt3-6", BUFFER);



			Dump_Simulacao(cpu_queue, cpu_ula, 0, ula_t3, "Operacao pt4-6", BUFFER);
			ULA_OUT = ULA_T1 * ULA_T2;

			Bus_Int = ULA_OUT >> 16;
			Registrador_AdicionaDado(Registradores_Geral, &Registradores_Geral->Registradores[Registrador_PegaID(Registradores_Geral, "DX")], Bus_Int);
			Dump_Simulacao(cpu_ula, cpu_registrador, Dump_Simulacao_registrador_indice("DX"), 0, "Operacao pt5-6", BUFFER);

			Bus_Int = ULA_OUT - Bus_Int << 16;
			Registrador_AdicionaDado(Registradores_Geral, &Registradores_Geral->Registradores[Registrador_PegaID(Registradores_Geral, "AX")], Bus_Int);
			Dump_Simulacao(cpu_ula, cpu_registrador, Dump_Simulacao_registrador_indice("AX"), 0, "Operacao pt6-6", BUFFER);
		}
	}

	void DIV(Registrador *Origem) {

		sprintf(BUFFER, "DIV %s", Origem->Nome);
		Bus_Int = Registrador_RecebeDado(Registradores_Geral,&Registradores_Geral->Registradores[Registrador_PegaID(Registradores_Geral, "AX")]);
		ULA_T1 = Bus_Int;
		Dump_Simulacao(cpu_registrador, cpu_ula, Dump_Simulacao_registrador_indice("AX"), ula_t1, "Operacao pt1-4", BUFFER);

		Bus_Int = Registrador_RecebeDado(Registradores_Geral, Origem);
		ULA_T2 = Bus_Int;
		Dump_Simulacao(cpu_registrador, cpu_ula, Dump_Simulacao_registrador_indice(Origem->Nome), ula_t2, "Operacao pt2-4", BUFFER);

		Dump_Simulacao(cpu_queue, cpu_ula, 0, ula_t3, "Operacao pt3-4", BUFFER);

		int Resto=0;
		int Operando=0;

		if(ULA_T2 != 0){
			ULA_OUT = ULA_T1 / ULA_T2;
			Operando = ULA_OUT;
			Resto = ULA_T1 - (ULA_T2 * Operando);
		}
		else {
			// flags!!!

		}
		/*
		when operand is a byte:
		AL = AX / operand
		AH = remainder (modulus)
		when operand is a word:
		AX = (DX AX) / operand
		DX = remainder (modulus)
		*/

		if (Origem->Tamanho == _16BITS) {

			ULA_OUT = Resto;
			Bus_Int = ULA_OUT;
			Dump_Simulacao(cpu_ula, cpu_registrador, Dump_Simulacao_registrador_indice("DX"), 0, "Operacao pt4-5", BUFFER);
			Registrador_AdicionaDado(Registradores_Geral, &Registradores_Geral->Registradores[Registrador_PegaID(Registradores_Geral, "DX")], Bus_Int);

			ULA_OUT = Operando;
			Bus_Int = ULA_OUT;
			Dump_Simulacao(cpu_ula, cpu_registrador, Dump_Simulacao_registrador_indice("AX"), 0, "Operacao pt5-5", BUFFER);
			Registrador_AdicionaDado(Registradores_Geral, &Registradores_Geral->Registradores[Registrador_PegaID(Registradores_Geral, "AX")], Bus_Int);
		}
		else {
			ULA_OUT = Operando + (Resto << 8);
			Bus_Int = ULA_OUT;
			Dump_Simulacao(cpu_ula, cpu_registrador, Dump_Simulacao_registrador_indice("AX"), 0, "Operacao pt4-4", BUFFER);
			Registrador_AdicionaDado(Registradores_Geral, &Registradores_Geral->Registradores[Registrador_PegaID(Registradores_Geral, "AX")], Bus_Int);
		}
	}

	void DIV(int Imediato, bool imediato) {

		sprintf(BUFFER, "DIV %d", Imediato);
		Bus_Int = Registrador_RecebeDado(Registradores_Geral, &Registradores_Geral->Registradores[Registrador_PegaID(Registradores_Geral, "AX")]);
		ULA_T1 = Bus_Int;
		Dump_Simulacao(cpu_queue, cpu_ula, 0, ula_t1, "Operacao pt1-5", BUFFER);

		Bus_Int = Imediato;
		ULA_T2 = Bus_Int;
		Dump_Simulacao(cpu_queue, cpu_ula, 0, ula_t2, "Operacao pt2-5", BUFFER);


		int Resto = 0;
		int Operando = 0;

		if (ULA_T2 != 0) {
			ULA_OUT = ULA_T1 / ULA_T2;
			Operando = ULA_OUT;
			Resto = ULA_T1 - (ULA_T2 * Operando);
		}
		else {
			// flags!!!

		}
		/*
		AX = (DX AX) / operand
		DX = remainder (modulus)
		*/
		Dump_Simulacao(cpu_queue, cpu_ula, 0, ula_t3, "Operacao pt3-5", BUFFER);
		ULA_OUT = Resto;
		Bus_Int = ULA_OUT;
		Registrador_AdicionaDado(Registradores_Geral, &Registradores_Geral->Registradores[Registrador_PegaID(Registradores_Geral, "DX")], Bus_Int);
		Dump_Simulacao(cpu_ula, cpu_registrador, Dump_Simulacao_registrador_indice("DX"), 0, "Operacao pt4-5", BUFFER);

		ULA_OUT = Operando;
		Bus_Int = ULA_OUT;
		Registrador_AdicionaDado(Registradores_Geral, &Registradores_Geral->Registradores[Registrador_PegaID(Registradores_Geral, "AX")], Bus_Int);
		Dump_Simulacao(cpu_ula, cpu_registrador, Dump_Simulacao_registrador_indice("AX"), 0, "Operacao pt5-5", BUFFER);
	}

	void DIV(int memoria) {


		if (memoria < 16) {
			char *DestinoBinario = InteiroParaCharBinario(memoria, _8BITS);
			int Id = Registrador_PegaID(Registradores_Geral, &DestinoBinario[4], true);// 8 - 3 = 5 bits, dos quais 1 é o final que indica o fim da string, então com isso isolamos
																					   //																					    o endereço do registrador que é 4 bits
			sprintf(BUFFER, "DIV [%s]", Registradores_Geral->Registradores[Id].Nome);
			Dump_Simulacao(cpu_registrador, cpu_queue, Dump_Simulacao_registrador_indice(Registradores_Geral->Registradores[Id].Nome), 0, "Operacao pt1-x", BUFFER);
			Bus_Int = Registrador_RecebeDado(Registradores_Geral, &Registradores_Geral->Registradores[Id]);
			Dump_Simulacao(cpu_queue, cpu_decode, 0, 0, "Operacao pt2-x", BUFFER);
			Dump_Simulacao(cpu_incrementa_ip, cpu_ip, 0, 0, "Operacao pt3-x", BUFFER);
			MUL(Bus_Int);
		}
		else {
			sprintf(BUFFER, "DIV %d", memoria);

			/*Não sei o que tem que ser mostrado D:*/
			Bus_Int = Memoria[memoria] << 8;
			Bus_Int += Memoria[memoria + 1];
			Dump_Simulacao(cpu_memoria, cpu_biu, 0, 0, "Operacao pt1-6", BUFFER);
			ULA_T2 = Bus_Int;
			Dump_Simulacao(cpu_biu, cpu_ula, 0, ula_t2, "Operacao pt2-6", BUFFER);

			Bus_Int = Registradores_Geral->Registradores[Registrador_PegaID(Registradores_Geral, "AX")].Dado;
			ULA_T1 = Bus_Int;
			Dump_Simulacao(cpu_queue, cpu_ula, 0, ula_t1, "Operacao pt3-6", BUFFER);



			int Resto = 0;
			int Operando = 0;

			if (ULA_T2 != 0) {
				ULA_OUT = ULA_T1 / ULA_T2;
				Operando = ULA_OUT;
				Resto = ULA_T1 - (ULA_T2 * Operando);
			}
			else {
				// flags!!!
			}
			/*
			AX = (DX AX) / operand
			DX = remainder (modulus)
			*/
			ULA_OUT = Resto;
			Dump_Simulacao(cpu_queue, cpu_ula, 0, ula_t3, "Operacao pt4-6", BUFFER);
			Bus_Int = ULA_OUT;
			Registrador_AdicionaDado(Registradores_Geral, &Registradores_Geral->Registradores[Registrador_PegaID(Registradores_Geral, "DX")], Bus_Int);
			Dump_Simulacao(cpu_ula, cpu_registrador, Dump_Simulacao_registrador_indice("DX"), 0, "Operacao pt5-6", BUFFER);
			ULA_OUT = Operando;
			Bus_Int = ULA_OUT;
			Registrador_AdicionaDado(Registradores_Geral, &Registradores_Geral->Registradores[Registrador_PegaID(Registradores_Geral, "AX")], Bus_Int);
			Dump_Simulacao(cpu_ula, cpu_registrador, Dump_Simulacao_registrador_indice("AX"), 0, "Operacao pt6-6", BUFFER);
		}
	}

	void INC(Registrador *Destino) {
		sprintf(BUFFER, "INC %s", Destino->Nome);
		ULA_T1 = Registrador_RecebeDado(Registradores_Geral, Destino);
		Dump_Simulacao(cpu_registrador, cpu_ula, Dump_Simulacao_registrador_indice(Destino->Nome), ula_t1, "Operacao pt1-3", BUFFER);
		ULA_OUT = ULA_T1 + 1;
		Dump_Simulacao(cpu_queue, cpu_ula, 0, ula_t3, "Operacao pt2-3", BUFFER);
		Bus_Int = ULA_OUT;
		Registrador_AdicionaDado(Registradores_Geral, Destino, Bus_Int);
		Dump_Simulacao(cpu_ula, cpu_registrador, Dump_Simulacao_registrador_indice(Destino->Nome), 0, "Operacao pt2-3", BUFFER);
	}

	void INC(int Destino) {
		sprintf(BUFFER, "INC [%d]", Destino);
		Bus_Int = Memoria[Destino];
		Dump_Simulacao(cpu_memoria, cpu_biu, 0,0, "Operacao pt1-5", BUFFER);
		ULA_T1 = Bus_Int;
		Dump_Simulacao(cpu_biu, cpu_ula, 0, ula_t1, "Operacao pt2-5", BUFFER);
		ULA_OUT = ULA_T1 + 1;
		Dump_Simulacao(cpu_queue, cpu_ula, 0, ula_t3, "Operacao pt3-5", BUFFER);
		Bus_Int = ULA_OUT;
		Dump_Simulacao(cpu_ula, cpu_biu, 0, 0, "Operacao pt4-5", BUFFER);
		Memoria[Destino] = Bus_Int;
		Cache_Salva_Dado(Memoria[Destino], Destino, _8BITS);
		Dump_Simulacao(cpu_biu, cpu_memoria, 0, 0, "Operacao pt5-5", BUFFER);
	}

	void DEC(Registrador *Destino) {
		sprintf(BUFFER, "DEC %s", Destino->Nome);
		ULA_T1 = Registrador_RecebeDado(Registradores_Geral, Destino);
		Dump_Simulacao(cpu_registrador, cpu_ula, Dump_Simulacao_registrador_indice(Destino->Nome), ula_t1, "Operacao pt1-3", BUFFER);
		ULA_OUT = ULA_T1 - 1;
		Dump_Simulacao(cpu_queue, cpu_ula, 0, ula_t3, "Operacao pt2-3", BUFFER);
		Bus_Int = ULA_OUT;
		Registrador_AdicionaDado(Registradores_Geral, Destino, Bus_Int);
		Dump_Simulacao(cpu_ula, cpu_registrador, Dump_Simulacao_registrador_indice(Destino->Nome), 0, "Operacao pt3-3", BUFFER);
	}

	void DEC(int Destino) {
		sprintf(BUFFER, "DEC [%d]", Destino);
		Dump_Simulacao(cpu_memoria, cpu_biu, 0, 0, "Operacao pt1-5", BUFFER);
		ULA_T1 = Memoria[Destino];
		Dump_Simulacao(cpu_queue, cpu_ula, 0, ula_t1, "Operacao pt2-5", BUFFER);
		ULA_OUT = ULA_T1 - 1;
		Dump_Simulacao(cpu_biu, cpu_ula, 0, ula_t3, "Operacao pt3-5", BUFFER);
		Bus_Int = ULA_OUT;
		Dump_Simulacao(cpu_ula, cpu_biu, 0, 0, "Operacao pt4-5", BUFFER);
		Memoria[Destino] = Bus_Int >> 8; // isola o high
		Memoria[Destino + 1] = Bus_Int - Memoria[Destino] << 8; // isola o low
		Cache_Salva_Dado(Memoria[Destino], Destino, _8BITS);
		Cache_Salva_Dado(Memoria[Destino+1], Destino+1, _8BITS);
		Dump_Simulacao(cpu_biu, cpu_memoria, 0, 0, "Operacao pt5-5", BUFFER);
	}
#pragma endregion

#pragma region DATA_TRANSFER
	void MOV(Registrador *Destino, Registrador *Origem) {//Regitrador Registrador
		Bus_Int = Registrador_RecebeDado(Registradores_Geral,Origem);
		Registrador_AdicionaDado(Registradores_Geral,Destino, Bus_Int);
		sprintf(BUFFER, "MOV %s %s", Destino->Nome, Origem->Nome);
		Dump_Simulacao(6, 6, Dump_Simulacao_registrador_indice(Origem->Nome), Dump_Simulacao_registrador_indice(Destino->Nome), "Operacao", BUFFER);
	}

	void MOV(Registrador *Origem, int Destino) {// Registrador Memoria
		sprintf(BUFFER, "MOV %s [%d]", Origem->Nome, Destino);
		Dump_Simulacao(0, 1, 0, 0, "Operacao pt1-2", BUFFER);
		if (Origem->Tamanho == _16BITS) {
			Bus_Int = Memoria[Destino];
			Bus_Int = Bus_Int << 8; // move a parte do high para cima (PRECISA SER TESTADO!!!)
			Bus_Int += Memoria[Destino + 1];// adiciona a parte do low ao bus int
		}
		else {
			Bus_Int = Memoria[Destino];
		}
		Registrador_AdicionaDado(Registradores_Geral, Origem, Bus_Int);
		Dump_Simulacao(0, 6, Dump_Simulacao_registrador_indice(Origem->Nome), 0, "Operacao pt2-2", BUFFER);
	}

	void MOV(int Origem, Registrador *Destino) {// Memoria Registrdor
		sprintf(BUFFER, "MOV [%d] %s", Origem, Destino->Nome);
		if (Destino->Tamanho == _16BITS) {
			Dump_Simulacao(6, 1, Dump_Simulacao_registrador_indice(Destino->Nome), 0, "Operacao pt1-4", BUFFER);
			Bus_Int = Registradores_Geral->Registradores[Destino->High_offset].Dado;
			Dump_Simulacao(1, 0, 0, 0, "Operacao pt2-4", BUFFER);
			sprintf(BUFFER, "MOV [%d] %s", Origem+1, Destino->Nome);
			Memoria[Origem] = Bus_Int;
			Cache_Salva_Dado(Bus_Int, Origem, _8BITS);
			Dump_Simulacao(6, 1, Dump_Simulacao_registrador_indice(Destino->Nome), 0, "Operacao pt3-4", BUFFER);
			Bus_Int = Registradores_Geral->Registradores[Destino->Low_offset].Dado;
			Memoria[Origem+1] = Bus_Int;
			Cache_Salva_Dado(Bus_Int, Origem + 1, _8BITS);

			Dump_Simulacao(1, 0, 0, 0, "Operacao pt4-4", BUFFER);
		}
		else {
			Bus_Int = Destino->Dado;
			Dump_Simulacao(6, 1, Dump_Simulacao_registrador_indice(Destino->Nome), 0, "Operacao pt1-2", BUFFER);
			Memoria[Origem] = Bus_Int;
			Cache_Salva_Dado(Bus_Int, Origem, _8BITS);
			Dump_Simulacao(1, 0, 0, 0, "Operacao pt2-2", BUFFER);
		}
	}

	void MOV(int Destino, int Origem) {// Memoria Imediato
		if (Destino < 16) {
			char *DestinoBinario = InteiroParaCharBinario(Destino, _8BITS);
			int Id = Registrador_PegaID(Registradores_Geral, &DestinoBinario[4], true);// 8 - 3 = 5 bits, dos quais 1 é o final que indica o fim da string, então com isso isolamos																					   //																					    o endereço do registrador que é 4 bits
			sprintf(BUFFER, "MOV [%s] %d", Registradores_Geral->Registradores[Id].Nome,Origem);
			Dump_Simulacao(cpu_registrador, cpu_queue, Dump_Simulacao_registrador_indice(Registradores_Geral->Registradores[Id].Nome), 0, "Operacao pt1-X", BUFFER);
			Bus_Int = Registrador_RecebeDado(Registradores_Geral, &Registradores_Geral->Registradores[Id]);
			Dump_Simulacao(cpu_queue, cpu_decode, 0, 0, "Operacao pt2-X", BUFFER);
			MOV(Bus_Int, Origem);
		}
		else {
			sprintf(BUFFER, "MOV [%d] %d", Destino, Origem);
			Bus_Int = Origem;
			Dump_Simulacao(cpu_queue, cpu_biu, 0, 0, "Operacao pt1-2", BUFFER);
			if (Origem > 255) {
				Memoria[Destino] = Bus_Int >> 8;
				Memoria[Destino + 1] = Bus_Int - Memoria[Destino] << 8;
				Cache_Salva_Dado(Memoria[Destino], Destino, _8BITS);
				Cache_Salva_Dado(Memoria[Destino + 1], Destino + 1, _8BITS);
			}
			else {
				Memoria[Destino] = Origem;
				Cache_Salva_Dado(Memoria[Destino], Destino, _8BITS);
			}
			Dump_Simulacao(cpu_biu, cpu_memoria, 0, 0, "Operacao pt2-2", BUFFER);
		}
	}

	void MOV(Registrador *Destino, int Valor,bool Val) {//Registrador Valor
		Bus_Int = Valor;
		Registrador_AdicionaDado(Registradores_Geral, Destino, Bus_Int);
		sprintf(BUFFER, "MOV %s %d", Destino->Nome, Valor);
		Dump_Simulacao(cpu_queue, cpu_registrador, Dump_Simulacao_registrador_indice(Destino->Nome), 0, "Operacao pt1-1", BUFFER);
	}

	void PUSH(int Imediato) {
		sprintf(BUFFER, "PUSH %d", Imediato);

		PILHA_PUSH_POP_ULTIMO++;
		if (PILHA_PUSH_POP_ULTIMO == 12) {
			PILHA_PUSH_POP_ULTIMO--;
		}
		PILHA_PUSH_POP[PILHA_PUSH_POP_ULTIMO] = Imediato;
		Dump_Simulacao(cpu_queue, cpu_decode, 0, 0, "Operacao pt1-1", BUFFER);
	}

	void PUSH(Registrador *dado) {
		sprintf(BUFFER, "PUSH %s", dado->Nome);


		Bus_Int = Registrador_RecebeDado(Registradores_Geral, dado);
		Dump_Simulacao(cpu_registrador, cpu_queue, Dump_Simulacao_registrador_indice(dado->Nome), 0, "Operacao pt1-2", BUFFER);
		PILHA_PUSH_POP_ULTIMO++;
		if (PILHA_PUSH_POP_ULTIMO == 12) {
			PILHA_PUSH_POP_ULTIMO--;
		}
		PILHA_PUSH_POP[PILHA_PUSH_POP_ULTIMO] = Bus_Int;
		Dump_Simulacao(cpu_queue, cpu_decode, 0, 0, "Operacao pt2-2", BUFFER);
	}

	void POP(int Imediato) {
		sprintf(BUFFER, "POP %d", Imediato);


		Dump_Simulacao(cpu_queue, cpu_decode, 0, 0, "Operacao pt1-1", BUFFER);

		PILHA_PUSH_POP_ULTIMO--;
		if (PILHA_PUSH_POP_ULTIMO == -2) {
			PILHA_PUSH_POP_ULTIMO++;
		}

		PILHA_PUSH_POP[PILHA_PUSH_POP_ULTIMO] = Imediato;


	}

	void POP(Registrador *dado) {
		sprintf(BUFFER, "PUSH %s", dado->Nome);


		Dump_Simulacao(cpu_decode, cpu_queue, 0, 0, "Operacao pt1-2", BUFFER);

		Bus_Int = PILHA_PUSH_POP[PILHA_PUSH_POP_ULTIMO];
		PILHA_PUSH_POP_ULTIMO--;
		if (PILHA_PUSH_POP_ULTIMO == -2) {
			PILHA_PUSH_POP_ULTIMO++;
		}
		Registrador_AdicionaDado(Registradores_Geral, dado, Bus_Int);

		Dump_Simulacao(cpu_queue, cpu_registrador, Dump_Simulacao_registrador_indice(dado->Nome), 0, "Operacao pt1-2", BUFFER);
	}
#pragma endregion
#pragma region LOGIC
	void NOT(Registrador *Destino) {
		sprintf(BUFFER, "NOT %s", Destino->Nome);
		Bus_Int = Registrador_RecebeDado(Registradores_Geral, Destino);
		ULA_T1 = Bus_Int;
		Dump_Simulacao(cpu_registrador, cpu_ula, Dump_Simulacao_registrador_indice(Destino->Nome), ula_t1, "Operacao pt1-3", BUFFER);
		ULA_OUT = ~ULA_T1;
		Dump_Simulacao(cpu_queue, cpu_biu, 0, ula_t3, "Operacao pt2-3", BUFFER);
		Bus_Int = ULA_OUT;
		Registrador_AdicionaDado(Registradores_Geral, Destino, Bus_Int);
		Dump_Simulacao(cpu_ula, cpu_registrador, Dump_Simulacao_registrador_indice(Destino->Nome), 0, "Operacao pt3-3", BUFFER);
	}

	void NOT(int Destino) {
		if (Destino < 16) {//a posição da memória está no registrador
			char *DestinoBinario = InteiroParaCharBinario(Destino, _8BITS);
			int Id = Registrador_PegaID(Registradores_Geral, &DestinoBinario[4], true);// 8 - 3 = 5 bits, dos quais 1 é o final que indica o fim da string, então com isso isolamos
			//																					    o endereço do registrador que é 4 bits
			sprintf(BUFFER, "NOT [%s]", Registradores_Geral->Registradores[Id].Nome);
			Dump_Simulacao(cpu_registrador, cpu_queue, Dump_Simulacao_registrador_indice(Registradores_Geral->Registradores[Id].Nome), 0, "Operacao pt1-X", BUFFER);
			Bus_Int = Registrador_RecebeDado(Registradores_Geral, &Registradores_Geral->Registradores[Id]);
			Dump_Simulacao(cpu_queue, cpu_decode, 0, 0, "Operacao pt2-X", BUFFER);
			NOT(Bus_Int);
		}
		else {
			//realmente é um endereço de memória
			sprintf(BUFFER, "NOT [%d]", Destino);
			Bus_Int = Memoria[Destino];
			Dump_Simulacao(cpu_memoria, cpu_biu, 0, 0, "Operacao pt1-5", BUFFER);
			ULA_T1 = Bus_Int;
			Dump_Simulacao(cpu_biu, cpu_ula, 0, ula_t1, "Operacao pt2-5", BUFFER);
			ULA_OUT = ~ULA_T1;
			Dump_Simulacao(cpu_queue, cpu_ula, 0, ula_t3, "Operacao pt3-5", BUFFER);
			Bus_Int = ULA_OUT;
			Dump_Simulacao(cpu_ula, cpu_biu, 0, 0, "Operacao pt4-5", BUFFER);
			Memoria[Destino] = Bus_Int >> 8; // isola o high
			Memoria[Destino + 1] = Bus_Int - Memoria[Destino] << 8; // isola o low
			Cache_Salva_Dado(Memoria[Destino], Destino, _8BITS);
			Cache_Salva_Dado(Memoria[Destino+1], Destino+1, _8BITS);
			Dump_Simulacao(cpu_biu, cpu_memoria, 0, 0, "Operacao pt5-5", BUFFER);

		}
	}

	void AND(int and1_memoria, Registrador *and2) {
		if (and1_memoria < 16) {
			char *DestinoBinario = InteiroParaCharBinario(and1_memoria, _8BITS);
			int Id = Registrador_PegaID(Registradores_Geral, &DestinoBinario[4], true);// 8 - 3 = 5 bits, dos quais 1 é o final que indica o fim da string, então com isso isolamos
																					   //																					    o endereço do registrador que é 4 bits
			sprintf(BUFFER, "AND [%s] %s", Registradores_Geral->Registradores[Id].Nome,and2->Nome);
			Dump_Simulacao(cpu_registrador, cpu_queue, Dump_Simulacao_registrador_indice(Registradores_Geral->Registradores[Id].Nome), 0, "Operacao pt1-X", BUFFER);
			Bus_Int = Registrador_RecebeDado(Registradores_Geral, &Registradores_Geral->Registradores[Id]);
			Dump_Simulacao(cpu_queue, cpu_decode, 0, 0, "Operacao pt2-X", BUFFER);
			AND(Bus_Int,and2);

		}else{
			sprintf(BUFFER, "AND [%d] %s", and1_memoria, and2->Nome);
			Dump_Simulacao(cpu_registrador, cpu_ula, Dump_Simulacao_registrador_indice(and2->Nome), ula_t1, "Operacao pt1-6", BUFFER);
			Bus_Int = Registrador_RecebeDado(Registradores_Geral, and2);
			ULA_T1 = Bus_Int;
			Dump_Simulacao(cpu_memoria, cpu_biu, 0, 0, "Operacao pt2-6", BUFFER);
			Bus_Int = Memoria[and1_memoria];
			Dump_Simulacao(cpu_biu, cpu_ula, 0, ula_t2, "Operacao pt3-6", BUFFER);
			ULA_T2 = Bus_Int;
			Dump_Simulacao(cpu_queue, cpu_ula, 0, ula_t3, "Operacao pt4-6", BUFFER);
			ULA_OUT = ULA_T1 & ULA_T2;
			Dump_Simulacao(cpu_ula, cpu_biu, 0, 0, "Operacao pt5-6", BUFFER);
			Bus_Int = ULA_OUT;

			if (and2->Tamanho == _8BITS) {
				Memoria[and1_memoria] = Bus_Int;
				Cache_Salva_Dado(Memoria[and1_memoria], and1_memoria, _8BITS);

			}
			else {
				//			Memoria[and1_memoria] = Bus_Int;
				Memoria[and1_memoria] = Registradores_Geral->Registradores[and2->High_offset].Dado;
				Memoria[and1_memoria + 1] = Registradores_Geral->Registradores[and2->Low_offset].Dado;
				Cache_Salva_Dado(Memoria[and1_memoria], and1_memoria, _8BITS);
				Cache_Salva_Dado(Memoria[and1_memoria+1], and1_memoria+1, _8BITS);

			}
			Dump_Simulacao(cpu_biu, cpu_memoria, 0, 0, "Operacao pt6-6", BUFFER);
		}
	}

	void AND(Registrador *and1, int and2_memoria) {
		if (and2_memoria < 16) {
			char *DestinoBinario = InteiroParaCharBinario(and2_memoria, _8BITS);
			int Id = Registrador_PegaID(Registradores_Geral, &DestinoBinario[4], true);// 8 - 3 = 5 bits, dos quais 1 é o final que indica o fim da string, então com isso isolamos
																					   //																					    o endereço do registrador que é 4 bits
			sprintf(BUFFER, "AND %s [%s]", and1->Nome, Registradores_Geral->Registradores[Id].Nome);
			Dump_Simulacao(cpu_registrador, cpu_queue, Dump_Simulacao_registrador_indice(Registradores_Geral->Registradores[Id].Nome), 0, "Operacao pt1-X", BUFFER);
			Bus_Int = Registrador_RecebeDado(Registradores_Geral, &Registradores_Geral->Registradores[Id]);
			Dump_Simulacao(cpu_queue, cpu_decode, 0, 0, "Operacao pt2-X", BUFFER);
			AND(and1, Bus_Int);

		}
		else {
			sprintf(BUFFER, "AND %s [%d]", and1->Nome, and2_memoria);
			Bus_Int = Registrador_RecebeDado(Registradores_Geral, and1);
			ULA_T1 = Bus_Int;
			Dump_Simulacao(cpu_registrador, cpu_ula, Dump_Simulacao_registrador_indice(and1->Nome), ula_t1, "Operacao pt1-6", BUFFER);

			Bus_Int = Memoria[and2_memoria];
			Dump_Simulacao(cpu_memoria, cpu_biu, 0, 0, "Operacao pt2-6", BUFFER);
			ULA_T2 = Bus_Int;
			Dump_Simulacao(cpu_biu, cpu_ula, 0, ula_t2, "Operacao pt3-6", BUFFER);
			ULA_OUT = ULA_T1 & ULA_T2;
			Dump_Simulacao(cpu_queue, cpu_ula, 0, ula_t3, "Operacao pt4-6", BUFFER);
			Bus_Int = ULA_OUT;
			Dump_Simulacao(cpu_ula, cpu_biu, 0, 0, "Operacao pt5-6", BUFFER);

			if (and1->Tamanho == _8BITS) {
				Memoria[and2_memoria] = Bus_Int;
			}
			else {
				//			Memoria[and1_memoria] = Bus_Int;
				Memoria[and2_memoria] = Registradores_Geral->Registradores[and1->High_offset].Dado;
				Memoria[and2_memoria + 1] = Registradores_Geral->Registradores[and1->Low_offset].Dado;
			}
			Dump_Simulacao(cpu_biu, cpu_memoria, 0, 0, "Operacao pt6-6", BUFFER);
		}
	}

	void AND(Registrador *Destino, Registrador *Origem) {
		sprintf(BUFFER, "AND %s %s", Destino->Nome, Origem->Nome);
		Bus_Int = Registrador_RecebeDado(Registradores_Geral, Destino);
		ULA_T1 = Bus_Int;
		Dump_Simulacao(cpu_registrador, cpu_ula, Dump_Simulacao_registrador_indice(Destino->Nome), ula_t1, "Operacao pt1-4", BUFFER);
		Bus_Int = Registrador_RecebeDado(Registradores_Geral, Origem);
		ULA_T2 = Bus_Int;
		Dump_Simulacao(cpu_registrador, cpu_ula, Dump_Simulacao_registrador_indice(Origem->Nome), ula_t2, "Operacao pt2-4", BUFFER);
		ULA_OUT = ULA_T1 & ULA_T2;
		Dump_Simulacao(cpu_queue, cpu_ula, 0, ula_t3, "Operacao pt3-4", BUFFER);
		Bus_Int = ULA_OUT;
		Registrador_AdicionaDado(Registradores_Geral, Destino, Bus_Int);
		Dump_Simulacao(cpu_ula, cpu_registrador, Dump_Simulacao_registrador_indice(Destino->Nome), 0, "Operacao pt4-4", BUFFER);
	}


	void AND(Registrador *and1, int and2_imediao,bool imediato) {
		ULA_OUT = ULA_T1 & ULA_T2;
	}

	void AND(int and_memoria, int and_imediato) {
		ULA_OUT = ULA_T1 & ULA_T2;
	}



	void OR(Registrador *Destino, Registrador *Origem) {
		sprintf(BUFFER, "OR %s %s", Destino->Nome, Origem->Nome);
		Bus_Int = Registrador_RecebeDado(Registradores_Geral, Destino);
		ULA_T1 = Bus_Int;
		Dump_Simulacao(cpu_registrador, cpu_ula, Dump_Simulacao_registrador_indice(Destino->Nome), ula_t1, "Operacao pt1-4", BUFFER);
		Bus_Int = Registrador_RecebeDado(Registradores_Geral, Origem);
		ULA_T2 = Bus_Int;
		Dump_Simulacao(cpu_registrador, cpu_ula, Dump_Simulacao_registrador_indice(Origem->Nome), ula_t2, "Operacao pt2-4", BUFFER);
		ULA_OUT = ULA_T1 | ULA_T2;
		Dump_Simulacao(cpu_queue, cpu_ula, 0, ula_t3, "Operacao pt3-4", BUFFER);
		Bus_Int = ULA_OUT;
		Registrador_AdicionaDado(Registradores_Geral, Destino, Bus_Int);
		Dump_Simulacao(cpu_ula, cpu_registrador, Dump_Simulacao_registrador_indice(Destino->Nome), 0, "Operacao pt4-4", BUFFER);
	}

	void XOR(Registrador *Destino, Registrador *Origem) {
		sprintf(BUFFER, "XOR %s %s", Destino->Nome, Origem->Nome);
		Bus_Int = Registrador_RecebeDado(Registradores_Geral, Destino);
		ULA_T1 = Bus_Int;
		Dump_Simulacao(cpu_registrador, cpu_ula, Dump_Simulacao_registrador_indice(Destino->Nome), ula_t1, "Operacao pt1-4", BUFFER);
		Bus_Int = Registrador_RecebeDado(Registradores_Geral, Origem);
		ULA_T2 = Bus_Int;
		Dump_Simulacao(cpu_registrador, cpu_ula, Dump_Simulacao_registrador_indice(Origem->Nome), ula_t2, "Operacao pt2-4", BUFFER);
		ULA_OUT = ULA_T1 ^ ULA_T2;
		Bus_Int = ULA_OUT;
		Dump_Simulacao(cpu_queue, cpu_ula, 0, ula_t3, "Operacao pt3-4", BUFFER);
		Registrador_AdicionaDado(Registradores_Geral, Destino, Bus_Int);
		Dump_Simulacao(cpu_ula, cpu_registrador, Dump_Simulacao_registrador_indice(Destino->Nome), 0, "Operacao pt4-4", BUFFER);
	}

#pragma endregion
#pragma region CONTROL
	void CMP_SETA_FLAGS() { //é chamado dentro da própria CPU
		int _CF = 0, _ZF = 0, _SF = 0, _OF = 0, _PF = 0, _AF = 0, _IF = 0, _DF = 0;
		int ID=0;
		int A1, A2;
		A1 = ULA_T1;
		A2 = ULA_T2;
		_asm {
			MOV EAX, A1
			MOV EBX, A2

			CMP AX, BX

			JE ZE_1 // Jump if first operand is Equal to second operand 
			ZE_0:
			JMP ZE_FIM
			ZE_1:
			MOV _ZF, 1
			ZE_FIM:

			JC JC_1 // Jump if Carry flag is set to 1
			JC_0:
			JMP JC_FIM
			JC_1:
			MOV _CF,1
			JC_FIM:
			
			JNS JNS_0 // Jump if Not Signed 
			JNS_1:
			MOV _SF,1
			JMP JNS_FIM
			JNS_0:
			JNS_FIM:
			
			JNO JNO_0 // jump not overflow
			JNO_1:
			MOV _OF,1
			JMP JNO_FIM
			JNO_0:
			JNO_FIM:

			JNP JNP_0 //Jump if No Parity 
			JNP_1:
			MOV _PF,1
			JMP JNP_FIM
			JNP_0:
			JNP_FIM:

			// não achei nenhuma maneira de testar o if nem o df
		}
		ID = Registrador_PegaID(Registrador_Flags, "CF");
		Registrador_Flags->Registradores[ID].Dado = _CF;
		ID = Registrador_PegaID(Registrador_Flags, "ZF");
		Registrador_Flags->Registradores[ID].Dado = _ZF;
		ID = Registrador_PegaID(Registrador_Flags, "SF");
		Registrador_Flags->Registradores[ID].Dado = _SF;
		ID = Registrador_PegaID(Registrador_Flags, "OF");
		Registrador_Flags->Registradores[ID].Dado = _OF;
		ID = Registrador_PegaID(Registrador_Flags, "PF");
		Registrador_Flags->Registradores[ID].Dado = _PF;
		ID = Registrador_PegaID(Registrador_Flags, "AF");
		Registrador_Flags->Registradores[ID].Dado = _AF;
		ID = Registrador_PegaID(Registrador_Flags, "IF");
		Registrador_Flags->Registradores[ID].Dado = _IF;
		ID = Registrador_PegaID(Registrador_Flags, "DF");
		Registrador_Flags->Registradores[ID].Dado = _DF;
	}


	void CMP(Registrador *Reg, int _Memoria) {//afeta as flags
		if (_Memoria > 16) {
			sprintf(BUFFER, "CMP %s [%d]", Reg->Nome, _Memoria);
			Bus_Int = Registrador_RecebeDado(Registradores_Geral, Reg);
			ULA_T1 = Bus_Int;
			Dump_Simulacao(cpu_registrador, cpu_ula, Dump_Simulacao_registrador_indice(Reg->Nome), ula_t1, "Operacao pt1-5", BUFFER);
			Dump_Simulacao(cpu_memoria, cpu_biu, 0, 0, "Operacao pt2-3", BUFFER);

			Bus_Int = Memoria[_Memoria];
			Bus_Int = Bus_Int << 8; // move a parte do high para cima (PRECISA SER TESTADO!!!)
			Bus_Int += Memoria[_Memoria + 1];// adiciona a parte do low ao bus int
			ULA_T2 = Bus_Int;
			Dump_Simulacao(cpu_biu, cpu_ula, 0, ula_t2, "Operacao pt3-5", BUFFER);
			Dump_Simulacao(cpu_queue, cpu_ula, 0, ula_t2, "Operacao pt4-5", BUFFER);
			CMP_SETA_FLAGS();
			Dump_Simulacao(cpu_queue, cpu_ula, 0, ula_t3, "Operacao pt5-5", BUFFER);
		}
		else {
			char *DestinoBinario = InteiroParaCharBinario(_Memoria, _8BITS);
			int Id = Registrador_PegaID(Registradores_Geral, &DestinoBinario[4], true);// 8 - 3 = 5 bits, dos quais 1 é o final que indica o fim da string, então com isso isolamos
																					   //o endereço do registrador que é 4 bits
			sprintf(BUFFER, "CMP %s [%s]", Reg->Nome, Registradores_Geral->Registradores[Id].Nome);
			Bus_Int = Registrador_RecebeDado(Registradores_Geral, &Registradores_Geral->Registradores[Id]);
			Dump_Simulacao(cpu_registrador, cpu_queue, Dump_Simulacao_registrador_indice(Registradores_Geral->Registradores[Id].Nome), 0, "Operacao pt1-X", BUFFER);
			Dump_Simulacao(cpu_queue, cpu_decode, 0, 0, "Operacao pt2-X", BUFFER);
			CMP(Reg, Bus_Int);
		}
	}

	void CMP(int _Memoria, Registrador *Reg) {//afeta as flags
		if (_Memoria > 16) {
			sprintf(BUFFER, "CMP [%d] %s", _Memoria, Reg->Nome);
			Dump_Simulacao(cpu_memoria, cpu_biu, 0, 0, "Operacao pt1-4", BUFFER);

			Bus_Int = Memoria[_Memoria];
			Bus_Int = Bus_Int << 8; // move a parte do high para cima (PRECISA SER TESTADO!!!)
			Bus_Int += Memoria[_Memoria + 1];// adiciona a parte do low ao bus int
			ULA_T1 = Bus_Int;
			Dump_Simulacao(cpu_biu, cpu_ula, 0, ula_t1, "Operacao pt2-4", BUFFER);

			Bus_Int = Registrador_RecebeDado(Registradores_Geral, Reg);
			ULA_T2 = Bus_Int;
			Dump_Simulacao(cpu_registrador, cpu_ula, Dump_Simulacao_registrador_indice(Reg->Nome), ula_t2, "Operacao pt3-4", BUFFER);

			CMP_SETA_FLAGS();
			Dump_Simulacao(cpu_queue, cpu_ula, 0, ula_t3, "Operacao pt4-4", BUFFER);
		}
		else {
			char *DestinoBinario = InteiroParaCharBinario(_Memoria, _8BITS);
			int Id = Registrador_PegaID(Registradores_Geral, &DestinoBinario[4], true);// 8 - 3 = 5 bits, dos quais 1 é o final que indica o fim da string, então com isso isolamos
																					   //o endereço do registrador que é 4 bits
			sprintf(BUFFER, "CMP [%s] %s", Registradores_Geral->Registradores[Id].Nome, Reg->Nome);
			Bus_Int = Registrador_RecebeDado(Registradores_Geral, &Registradores_Geral->Registradores[Id]);
			Dump_Simulacao(cpu_registrador, cpu_queue, Dump_Simulacao_registrador_indice(Registradores_Geral->Registradores[Id].Nome), 0, "Operacao pt1-X", BUFFER);
			Dump_Simulacao(cpu_queue, cpu_decode, 0, 0, "Operacao pt2-X", BUFFER);
			CMP(Bus_Int, Reg);
		}

	}

	void CMP(Registrador *Reg1, Registrador *Reg2) {//afeta as flags
		sprintf(BUFFER, "CMP %s %s", Reg1->Nome, Reg2->Nome);
		Bus_Int = Registrador_RecebeDado(Registradores_Geral, Reg1);
		ULA_T1 = Bus_Int;
		Dump_Simulacao(cpu_registrador, cpu_ula, Dump_Simulacao_registrador_indice(Reg1->Nome), ula_t1, "Operacao pt1-3", BUFFER);
		Bus_Int = Registrador_RecebeDado(Registradores_Geral, Reg2);
		ULA_T2 = Bus_Int;
		Dump_Simulacao(cpu_registrador, cpu_ula, Dump_Simulacao_registrador_indice(Reg2->Nome), ula_t2, "Operacao pt2-3", BUFFER);
		CMP_SETA_FLAGS();
		Dump_Simulacao(cpu_queue, cpu_ula, 0, ula_t3, "Operacao pt3-3", BUFFER);


	}

	void CMP(int _Memoria, int Imediato) {//afeta as flags
		if (_Memoria > 16) {
			sprintf(BUFFER, "CMP [%d] %d", _Memoria, Imediato);
			Dump_Simulacao(cpu_memoria, cpu_biu, 0, 0, "Operacao pt1-4", BUFFER);

			Bus_Int = Memoria[_Memoria];
			Bus_Int = Bus_Int << 8; // move a parte do high para cima (PRECISA SER TESTADO!!!)
			Bus_Int += Memoria[_Memoria + 1];// adiciona a parte do low ao bus int
			ULA_T1 = Bus_Int;
			Dump_Simulacao(cpu_biu, cpu_ula, 0, ula_t1, "Operacao pt2-4", BUFFER);

			Bus_Int = Imediato;
			ULA_T2 = Bus_Int;
			Dump_Simulacao(cpu_queue, cpu_ula, 0, ula_t2, "Operacao pt3-4", BUFFER);

			CMP_SETA_FLAGS();
			Dump_Simulacao(cpu_queue, cpu_ula, 0, ula_t3, "Operacao pt4-4", BUFFER);
		}
		else {
			char *DestinoBinario = InteiroParaCharBinario(_Memoria, _8BITS);
			int Id = Registrador_PegaID(Registradores_Geral, &DestinoBinario[4], true);// 8 - 3 = 5 bits, dos quais 1 é o final que indica o fim da string, então com isso isolamos
																					   //o endereço do registrador que é 4 bits
			sprintf(BUFFER, "CMP [%s] %d", Registradores_Geral->Registradores[Id].Nome, Imediato);
			Bus_Int = Registrador_RecebeDado(Registradores_Geral, &Registradores_Geral->Registradores[Id]);
			Dump_Simulacao(cpu_registrador, cpu_queue, Dump_Simulacao_registrador_indice(Registradores_Geral->Registradores[Id].Nome), 0, "Operacao pt1-X", BUFFER);
			Dump_Simulacao(cpu_queue, cpu_decode, 0, 0, "Operacao pt2-X", BUFFER);
			CMP(Bus_Int,Imediato);
		}

	}

	void CMP(Registrador *Reg, int Imediato, bool imediado_) {//afeta as flags
		sprintf(BUFFER, "CMP %s %d", Reg->Nome, Imediato);
		Bus_Int = Registrador_RecebeDado(Registradores_Geral, Reg);
		ULA_T1 = Bus_Int;
		Dump_Simulacao(cpu_registrador, cpu_ula, Dump_Simulacao_registrador_indice(Reg->Nome), ula_t1, "Operacao pt1-3", BUFFER);
		Bus_Int = Imediato;
		ULA_T2 = Bus_Int;
		Dump_Simulacao(cpu_queue, cpu_ula, 0, ula_t2, "Operacao pt2-3", BUFFER);
		CMP_SETA_FLAGS();
		Dump_Simulacao(cpu_queue, cpu_ula, 0, ula_t3, "Operacao pt3-3", BUFFER);
	}

	void CALL(Registrador *Reg) {
		sprintf(BUFFER, "CALL %s", Reg->Nome);
		Dump_Simulacao(cpu_registrador, cpu_queue, Dump_Simulacao_registrador_indice(Reg->Nome), 0, "Operacao pt1-3", BUFFER);
		Bus_Int = Registrador_RecebeDado(Registradores_Geral, Reg);
		Dump_Simulacao(cpu_queue, cpu_decode, 0, 0, "Operacao pt2-3", BUFFER);
		if (IP->Registradores[IP->Quantidade - 1].Dado != 0) {
			MT2D_MessageBox("Estouro de pilha IP");
		}
		for (int i = IP->Quantidade - 1; i > 0; i--) {
			IP->Registradores[i].Dado = IP->Registradores[i - 1].Dado;
		}
		IP->Registradores[0].Dado = Bus_Int - 2;//o + 2 será feito depois da simulação
		Dump_Simulacao(cpu_incrementa_ip, cpu_ip, 0, 0, "Operacao pt3-3", BUFFER);
	}

	void CALL(int memoria) {
		if (memoria < 16) {
			char *DestinoBinario = InteiroParaCharBinario(memoria, _8BITS);
			int Id = Registrador_PegaID(Registradores_Geral, &DestinoBinario[4], true);// 8 - 3 = 5 bits, dos quais 1 é o final que indica o fim da string, então com isso isolamos
																					   //																					    o endereço do registrador que é 4 bits
			sprintf(BUFFER, "CALL [%s]", Registradores_Geral->Registradores[Id].Nome);
			Dump_Simulacao(cpu_registrador, cpu_queue, Dump_Simulacao_registrador_indice(Registradores_Geral->Registradores[Id].Nome), 0, "Operacao pt1-x", BUFFER);
			Bus_Int = Registrador_RecebeDado(Registradores_Geral, &Registradores_Geral->Registradores[Id]);
			Dump_Simulacao(cpu_queue, cpu_decode, 0, 0, "Operacao pt2-x", BUFFER);
			Dump_Simulacao(cpu_incrementa_ip, cpu_ip, 0, 0, "Operacao pt3-x", BUFFER);
			CALL(Bus_Int);
		}
		else {
			sprintf(BUFFER, "CALL [%d]", Memoria);
			if (IP->Registradores[IP->Quantidade - 1].Dado != 0) {
				MT2D_MessageBox("Estouro de pilha IP");
			}
			for (int i = IP->Quantidade-1; i > 0 ; i--) {
				IP->Registradores[i].Dado = IP->Registradores[i - 1].Dado;
			}
			/*Não sei o que tem que ser mostrado D:*/
			Dump_Simulacao(cpu_memoria, cpu_biu, 0, 0, "Operacao pt1-4", BUFFER);
			Bus_Int = Memoria[memoria] << 8;
			Bus_Int += Memoria[memoria + 1];
			Dump_Simulacao(cpu_biu, cpu_queue, 0, 0, "Operacao pt2-4", BUFFER);
			Dump_Simulacao(cpu_queue, cpu_decode, 0, 0, "Operacao pt3-4", BUFFER);
			IP->Registradores[0].Dado = Bus_Int - 2;//o + 2 será feito depois da simulação
			Dump_Simulacao(cpu_incrementa_ip, cpu_ip, 0, 0, "Operacao pt4-4", BUFFER);
		}
	}

	void CALL(int Imediato, bool imediato_tag) {
		sprintf(BUFFER, "CALL %d", Imediato);
		if (IP->Registradores[IP->Quantidade - 1].Dado != 0) {
			MT2D_MessageBox("Estouro de pilha IP");
		}
		for (int i = IP->Quantidade - 1; i > 0; i--) {
			IP->Registradores[i].Dado = IP->Registradores[i - 1].Dado;
		}
		/*Não sei o que tem que ser mostrado D:*/
		IP->Registradores[0].Dado = Imediato - 2;//o + 2 será feito depois da simulação
		Dump_Simulacao(cpu_incrementa_ip, cpu_ip, 0, 0, "Operacao pt1-1", BUFFER);

	}


	void JMP(Registrador *PularPara) {
		/*Não sei o que tem que ser mostrado D:*/
		sprintf(BUFFER, "JMP  %s", PularPara->Nome);
		Dump_Simulacao(cpu_registrador, cpu_queue, Dump_Simulacao_registrador_indice(PularPara->Nome), 0, "Operacao pt1-x", BUFFER);
		Bus_Int = Registrador_RecebeDado(Registradores_Geral, PularPara);
		Dump_Simulacao(cpu_queue, cpu_decode, 0, 0, "Operacao pt2-x", BUFFER);
		IP->Registradores[0].Dado = Bus_Int - 2;//o + 2 será feito depois da simulação
		Dump_Simulacao(cpu_incrementa_ip, cpu_ip, 0, 0, "Operacao pt3-x", BUFFER);
	}

	void JMP(int PularPara) {
		if (PularPara< 16) {//a posição da memória está no registrador
			char *DestinoBinario = InteiroParaCharBinario(PularPara, _8BITS);
			int Id = Registrador_PegaID(Registradores_Geral, &DestinoBinario[4], true);// 8 - 3 = 5 bits, dos quais 1 é o final que indica o fim da string, então com isso isolamos
																					   //																					    o endereço do registrador que é 4 bits
			sprintf(BUFFER, "JMP [%s]", Registradores_Geral->Registradores[Id].Nome);
			Dump_Simulacao(cpu_registrador, cpu_queue, Dump_Simulacao_registrador_indice(Registradores_Geral->Registradores[Id].Nome), 0, "Operacao pt1-3", BUFFER);
			Bus_Int = Registrador_RecebeDado(Registradores_Geral, &Registradores_Geral->Registradores[Id]);
			Dump_Simulacao(cpu_queue, cpu_decode, 0, 0, "Operacao pt2-3", BUFFER);
			IP->Registradores[0].Dado = Bus_Int - 2;//o + 2 será feito depois da simulação
			Dump_Simulacao(cpu_incrementa_ip, cpu_ip, 0, 0, "Operacao pt3-3", BUFFER);
		}
		else {
			/*Não sei o que tem que ser mostrado D:*/
			sprintf(BUFFER, "JMP [%d]", PularPara);
			Dump_Simulacao(cpu_memoria, cpu_biu, 0, 0, "Operacao pt1-4", BUFFER);
			Bus_Int = PularPara;
			Dump_Simulacao(cpu_biu, cpu_queue, 0, 0, "Operacao pt2-4", BUFFER);
			Dump_Simulacao(cpu_queue, cpu_decode, 0, 0, "Operacao pt3-4", BUFFER);
			IP->Registradores[0].Dado = Bus_Int - 2;//o + 2 será feito depois da simulação
			Dump_Simulacao(cpu_incrementa_ip, cpu_ip, 0, 0, "Operacao pt4-4", BUFFER);
		}
	}

	void JMP(int PularPara, bool Imediato) {
			/*Não sei o que tem que ser mostrado D:*/
			sprintf(BUFFER, "JMP %d", PularPara);
			Dump_Simulacao(cpu_queue, cpu_decode, 0, 0, "Operacao pt1-2", BUFFER);
			IP->Registradores[0].Dado = PularPara - 2;//o + 2 será feito depois da simulação
			Dump_Simulacao(cpu_incrementa_ip, cpu_ip, 0, 0, "Operacao pt2-2", BUFFER);
	}


	void RET() {
		sprintf(BUFFER, "RET");

		if (IP->Registradores[IP->Quantidade - 1].Dado != 0) {
			MT2D_MessageBox("Estouro de pilha IP");
		}
		if (IP->Registradores[1].Dado > 0) {
			for (int i = 0; i < IP->Quantidade - 1; i++) {
				IP->Registradores[i].Dado = IP->Registradores[i+1].Dado;
				IP->Registradores[i + 1].Dado = 0;
			}
		}
		else {
			IP->Registradores[0].Dado = 0;
		}
		Dump_Simulacao(cpu_incrementa_ip, cpu_ip, 0, 0, "Operacao pt1-1", BUFFER);

	}

	void JE(int Salto) {// Jump Equal
		sprintf(BUFFER, "JE %d", Salto);
		int Flag_Id = Registrador_PegaID(Registrador_Flags, "ZF");
		if (Registrador_Flags->Registradores[Flag_Id].Dado == true) {// é igual
			Dump_Simulacao(cpu_queue, cpu_ula, 0, ula_t3, "Operacao pt1-2", BUFFER);
			IP->Registradores[0].Dado = Salto - 2;//o + 2 será feito depois da simulação
			Dump_Simulacao(cpu_incrementa_ip, cpu_ip, 0, 0, "Operacao pt2-2", BUFFER);
		}
		else {// é diferente
			Dump_Simulacao(cpu_queue, cpu_ula, 0, ula_t3, "Operacao pt1-1", BUFFER);

		}
	}

	void JNE(int Salto) {// Jump Equal
		sprintf(BUFFER, "JNE %d", Salto);
		int Flag_Id = Registrador_PegaID(Registrador_Flags, "ZF");
		if (Registrador_Flags->Registradores[Flag_Id].Dado == false) {// é igual
			Dump_Simulacao(cpu_queue, cpu_ula, 0, ula_t3, "Operacao pt1-2", BUFFER);
			IP->Registradores[0].Dado = Salto - 2;//o + 2 será feito depois da simulação
			Dump_Simulacao(cpu_incrementa_ip, cpu_ip, 0, 0, "Operacao pt2-2", BUFFER);
		}
		else {// é diferente
			Dump_Simulacao(cpu_queue, cpu_ula, 0, ula_t3, "Operacao pt1-1", BUFFER);

		}
	}

	void JNZ(int Salto) {// Jump Equal
		sprintf(BUFFER, "JNZ %d", Salto);
		int Flag_Id = Registrador_PegaID(Registrador_Flags, "ZF");
		if (Registrador_Flags->Registradores[Flag_Id].Dado == false) {// é igual
			Dump_Simulacao(cpu_queue, cpu_ula, 0, ula_t3, "Operacao pt1-2", BUFFER);
			IP->Registradores[0].Dado = Salto - 2;//o + 2 será feito depois da simulação
			Dump_Simulacao(cpu_incrementa_ip, cpu_ip, 0, 0, "Operacao pt2-2", BUFFER);
		}
		else {// é diferente
			Dump_Simulacao(cpu_queue, cpu_ula, 0, ula_t3, "Operacao pt1-1", BUFFER);

		}
	}

	void JZ(int Salto) {// Jump Zero
		sprintf(BUFFER, "JZ %d", Salto);
		int Flag_Id = Registrador_PegaID(Registrador_Flags, "ZF");
		if (Registrador_Flags->Registradores[Flag_Id].Dado == true) {// é igual
			Dump_Simulacao(cpu_queue, cpu_ula, 0, ula_t3, "Operacao pt1-2", BUFFER);
			IP->Registradores[0].Dado = Salto - 2;//o + 2 será feito depois da simulação
			Dump_Simulacao(cpu_incrementa_ip, cpu_ip, 0, 0, "Operacao pt2-2", BUFFER);
		}
		else {// é diferente
			Dump_Simulacao(cpu_queue, cpu_ula, 0, ula_t3, "Operacao pt1-1", BUFFER);

		}
	}

	void JL(int Salto) { // Jump Lower
		sprintf(BUFFER, "JL %d", Salto);
		int Flag_Id1 = Registrador_PegaID(Registrador_Flags, "SF");
		int Flag_Id2 = Registrador_PegaID(Registrador_Flags, "OF");
		if (Registrador_Flags->Registradores[Flag_Id1].Dado != Registrador_Flags->Registradores[Flag_Id2].Dado) {// faz jump
			Dump_Simulacao(cpu_queue, cpu_ula, 0, ula_t3, "Operacao pt1-2", BUFFER);
			IP->Registradores[0].Dado = Salto - 2;//o + 2 será feito depois da simulação
			Dump_Simulacao(cpu_incrementa_ip, cpu_ip, 0, 0, "Operacao pt2-2", BUFFER);
		}
		else {// é diferente
			Dump_Simulacao(cpu_queue, cpu_ula, 0, ula_t3, "Operacao pt1-1", BUFFER);

		}
	}

	void JNGE(int Salto) {// Jump not Great Equal
		sprintf(BUFFER, "JNGE %d", Salto);
		int Flag_Id1 = Registrador_PegaID(Registrador_Flags, "SF");
		int Flag_Id2 = Registrador_PegaID(Registrador_Flags, "OF");
		if (Registrador_Flags->Registradores[Flag_Id1].Dado !=  Registrador_Flags->Registradores[Flag_Id2].Dado) {// faz jump
			Dump_Simulacao(cpu_queue, cpu_ula, 0, ula_t3, "Operacao pt1-2", BUFFER);
			IP->Registradores[0].Dado = Salto - 2;//o + 2 será feito depois da simulação
			Dump_Simulacao(cpu_incrementa_ip, cpu_ip, 0, 0, "Operacao pt2-2", BUFFER);
		}
		else {// é diferente
			Dump_Simulacao(cpu_queue, cpu_ula, 0, ula_t3, "Operacao pt1-1", BUFFER);

		}
	}

	void JA(int Salto) {// Jump Above
		sprintf(BUFFER, "JA %d", Salto);
		int Flag_Id1 = Registrador_PegaID(Registrador_Flags, "CF");
		int Flag_Id2 = Registrador_PegaID(Registrador_Flags, "ZF");
		if (Registrador_Flags->Registradores[Flag_Id1].Dado == 0 &&  Registrador_Flags->Registradores[Flag_Id2].Dado == 0) {// faz jump
			Dump_Simulacao(cpu_queue, cpu_ula, 0, ula_t3, "Operacao pt1-2", BUFFER);
			IP->Registradores[0].Dado = Salto - 2;//o + 2 será feito depois da simulação
			Dump_Simulacao(cpu_incrementa_ip, cpu_ip, 0, 0, "Operacao pt2-2", BUFFER);
		}
		else {// é diferente
			Dump_Simulacao(cpu_queue, cpu_ula, 0, ula_t3, "Operacao pt1-1", BUFFER);

		}

	}

	void JNBE(int Salto) {// Jump Not Below Equal
		sprintf(BUFFER, "JNBE %d", Salto);
		int Flag_Id1 = Registrador_PegaID(Registrador_Flags, "CF");
		int Flag_Id2 = Registrador_PegaID(Registrador_Flags, "ZF");
		if (Registrador_Flags->Registradores[Flag_Id1].Dado == 0 && Registrador_Flags->Registradores[Flag_Id2].Dado == 0) {// faz jump
			Dump_Simulacao(cpu_queue, cpu_ula, 0, ula_t3, "Operacao pt1-2", BUFFER);
			IP->Registradores[0].Dado = Salto - 2;//o + 2 será feito depois da simulação
			Dump_Simulacao(cpu_incrementa_ip, cpu_ip, 0, 0, "Operacao pt2-2", BUFFER);
		}
		else {// é diferente
			Dump_Simulacao(cpu_queue, cpu_ula, 0, ula_t3, "Operacao pt1-1", BUFFER);

		}
	}

	void JB(int Salto) { // Jump Below
		sprintf(BUFFER, "JB %d", Salto);
		int Flag_Id = Registrador_PegaID(Registrador_Flags, "CF");
		if (Registrador_Flags->Registradores[Flag_Id].Dado == true) {// é igual
			Dump_Simulacao(cpu_queue, cpu_ula, 0, ula_t3, "Operacao pt1-2", BUFFER);
			IP->Registradores[0].Dado = Salto - 2;//o + 2 será feito depois da simulação
			Dump_Simulacao(cpu_incrementa_ip, cpu_ip, 0, 0, "Operacao pt2-2", BUFFER);
		}
		else {// é diferente
			Dump_Simulacao(cpu_queue, cpu_ula, 0, ula_t3, "Operacao pt1-1", BUFFER);

		}
	}

	void JNAE(int Salto) { // Jump not Above Equal
		sprintf(BUFFER, "JNAE %d", Salto);
		int Flag_Id = Registrador_PegaID(Registrador_Flags, "CF");
		if (Registrador_Flags->Registradores[Flag_Id].Dado == true) {// é igual
			Dump_Simulacao(cpu_queue, cpu_ula, 0, ula_t3, "Operacao pt1-2", BUFFER);
			IP->Registradores[0].Dado = Salto - 2;//o + 2 será feito depois da simulação
			Dump_Simulacao(cpu_incrementa_ip, cpu_ip, 0, 0, "Operacao pt2-2", BUFFER);
		}
		else {// é diferente
			Dump_Simulacao(cpu_queue, cpu_ula, 0, ula_t3, "Operacao pt1-1", BUFFER);

		}
	}

	void JG(int Salto) {
		int Flag_Id1 = Registrador_PegaID(Registrador_Flags, "ZF");
		int Flag_Id2 = Registrador_PegaID(Registrador_Flags, "SF");
		int Flag_Id3 = Registrador_PegaID(Registrador_Flags, "OF");
		sprintf(BUFFER, "JG %d", Salto);
		if (Registrador_Flags->Registradores[Flag_Id1].Dado == 0 && Registrador_Flags->Registradores[Flag_Id2].Dado == Registrador_Flags->Registradores[Flag_Id3].Dado) {// faz jump
			Dump_Simulacao(cpu_queue, cpu_ula, 0, ula_t3, "Operacao pt1-2", BUFFER);
			IP->Registradores[0].Dado = Salto - 2;//o + 2 será feito depois da simulação
			Dump_Simulacao(cpu_incrementa_ip, cpu_ip, 0, 0, "Operacao pt2-2", BUFFER);

		}
		else {// é diferente
			Dump_Simulacao(cpu_queue, cpu_ula, 0, ula_t3, "Operacao pt1-1", BUFFER);
		}
	}

	void JNLE(int Salto) {
		int Flag_Id1 = Registrador_PegaID(Registrador_Flags, "ZF");
		int Flag_Id2 = Registrador_PegaID(Registrador_Flags, "SF");
		int Flag_Id3 = Registrador_PegaID(Registrador_Flags, "OF");
		sprintf(BUFFER, "JNLE %d", Salto);
		if (Registrador_Flags->Registradores[Flag_Id1].Dado == 0 && Registrador_Flags->Registradores[Flag_Id2].Dado == Registrador_Flags->Registradores[Flag_Id3].Dado) {// faz jump
			Dump_Simulacao(cpu_queue, cpu_ula, 0, ula_t3, "Operacao pt1-2", BUFFER);
			IP->Registradores[0].Dado = Salto - 2;//o + 2 será feito depois da simulação
			Dump_Simulacao(cpu_incrementa_ip, cpu_ip, 0, 0, "Operacao pt2-2", BUFFER);

		}
		else {// é diferente
			Dump_Simulacao(cpu_queue, cpu_ula, 0, ula_t3, "Operacao pt1-1", BUFFER);
		}
	}


#pragma endregion

};


extern CPU i8086;
#endif