//==========================================
//          MAIN DO SIMULADOR
//==========================================
#include <stdio.h>
#include <stdlib.h>
#include "../../MT2D/Window_core.h"
#include "../../MT2D/building_functions/generic_string_reader.h"
#include "../../MT2D/building_functions/display_popup.h"
#include "CPU.h"
#include "Instrucao.h"
#include "Registrador.h"
#include "Texto.h"
#include "Dump_Memoria.h"
#include "Dump_Registrador.h"

unsigned char Memoria[65535];// 1 byte por char -> 64KB ram
char *ArquivoFonte = 0;
FILE *Arquivo=0;
CPU i8086;

void Smain() {
	int i,j, k;
	char BUFFER_BYTE[9];
#pragma region PARTE1
	create_window_layout(0, 0, MAX_HOR - 1, MAX_VER - 1, false, false, 0, ' ', DISPLAY_WINDOW1);
	insert_string_on_display("Simulador", 0, 10, DISPLAY_WINDOW1);
	print_display(DISPLAY_WINDOW1);
	i8086.Instrucoes = CriaTabelaInstrucao();
	i8086.Registradores_Geral = CriaTabelaRegistrador();

	
	i8086.Registradores_Segmento = CriaTabelaRegistrador();
	AdicionaTabela(i8086.Registradores_Segmento, "CS", _16BITS, NAO,NULL,NULL,"0000");
	AdicionaTabela(i8086.Registradores_Segmento, "DS", _16BITS, NAO, NULL, NULL, "0000");
	AdicionaTabela(i8086.Registradores_Segmento, "ES", _16BITS, NAO, NULL, NULL, "0000");
	AdicionaTabela(i8086.Registradores_Segmento, "SS", _16BITS, NAO, NULL, NULL, "0000");

	i8086.Registrador_Flags = CriaTabelaRegistrador();
	AdicionaTabela(i8086.Registrador_Flags, "CF", _8BITS, NAO, NULL, NULL, "0000");
	AdicionaTabela(i8086.Registrador_Flags, "PF", _8BITS, NAO, NULL, NULL, "0000");
	AdicionaTabela(i8086.Registrador_Flags, "AF", _8BITS, NAO, NULL, NULL, "0000");
	AdicionaTabela(i8086.Registrador_Flags, "ZF", _8BITS, NAO, NULL, NULL, "0000");
	AdicionaTabela(i8086.Registrador_Flags, "SF", _8BITS, NAO, NULL, NULL, "0000");
	AdicionaTabela(i8086.Registrador_Flags, "TF", _8BITS, NAO, NULL, NULL, "0000");
	AdicionaTabela(i8086.Registrador_Flags, "IF", _8BITS, NAO, NULL, NULL, "0000");
	AdicionaTabela(i8086.Registrador_Flags, "DF", _8BITS, NAO, NULL, NULL, "0000");
	AdicionaTabela(i8086.Registrador_Flags, "OF", _8BITS, NAO, NULL, NULL, "0000");

	i8086.IP = CriaTabelaRegistrador();
	AdicionaTabela(i8086.IP, "IP", _16BITS, SIM, NULL, NULL, "0000");
	AdicionaTabela(i8086.IP, "PILHA_IP", _16BITS, SIM, NULL, NULL, "0000");

	i8086.IP->Registradores[0].Dado = 256;
	i8086.IP->Registradores[1].Dado = 0;


#pragma endregion - Inicializa as estruturas da simulação
	if (!i8086.Instrucoes && !i8086.Registradores_Geral) {
		print_popup("Instrucoes ou Registradores nao encontrados, simulador abortado");
	}
	else {
		do {
			ArquivoFonte = get_string_popup("Digite o endereco e\\ou o nome do arquito");
			if (!ArquivoFonte) {
				print_popup("Nada foi digitado");
			}
			else {
				Arquivo = fopen(ArquivoFonte, "r");
				if (!Arquivo) {
					free(ArquivoFonte);
					print_popup("Arquivo nao encontrado");
				}
			}

		} while (!Arquivo);
#pragma region PARTE 2
		for (i = 0; i < 65535; i++) {//limpa a memória
			Memoria[i] = 0;
		}
		k = 256;//100h
		while (!feof(Arquivo)) {
			for (j = 0; j < 8 && !feof(Arquivo); j++) {// captura 2 bytes
				BUFFER_BYTE[j] = fgetc(Arquivo);
			}
			BUFFER_BYTE[j] = '\0';
			Memoria[k] = CharBinarioParaInteiro(BUFFER_BYTE);//carrega o programa na memória
			k++;
		}
#pragma endregion - Carrega o programa na memória
	}
	while (i8086.IP->Registradores[0].Dado < 300) {
		Dump_Memoria();
		Dump_Registrador();
		i8086.IP->Registradores[0].Dado += 3;
	}
	LimpaTabela(i8086.Instrucoes);
	LimpaTabela(i8086.Registradores_Segmento);
	LimpaTabela(i8086.Registrador_Flags);
	LimpaTabela(i8086.IP);
}