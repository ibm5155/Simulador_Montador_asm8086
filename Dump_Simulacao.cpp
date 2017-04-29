#include "Texto.h"
#include <MT2D/MT2D.h>
#include <MT2D/MT2D_Display.h>
#include <MT2D/MT2D_Layout.h>
#include <MT2D/MT2D_Keyboard.h>

#include <MT2D/MT2D_System_Calls.h>
#include <MT2D/MessageBox/MT2D_MessageBox.h>

#include "RAM.h"
#include "CPU.h"
#include "Smain.h"
#include <stdlib.h>
#include <string.h>

extern int Velocidade_Simulador;

void Desenha_Estrutura_Hardware();


void Move_Memoria_Bio();
void Move_Bio_Memoria();
void Move_Bio_Queue();
void Move_Bio_Registrador_Inicio();
void Move_Bio_ULA_Inicio();
void Move_Queue_Controle();
void Move_Queue_Registrador_Inicio();
void Move_Queue_ULA_Inicio();
void Move_Queue_Bio();
void Move_IP_BIO();
void Move_INC_IP();
void Move_Registrador_Inicio_Registrador(int Registrador);
void Move_Registrador_ULA_Inicio(int Registrador);
void Move_Registrador_Queue(int Registrador);
void Move_Registrador_Bio(int Registrador);
void Move_Registrador_Registrador_Inicio(int Registrador1);
void Move_ULA_Inicio_Tx(int Tx);
void Move_ULA_Zo();
void Move_ULA_Registrador_Inicio();
void Move_ULA_Queue();
void Move_ULA_Bio();

char *strings, *strgings2;

int Dump_Simulacao_registrador_indice(char *Nome_Registrador) {
	int reg = 0;
	if (strcmp(Nome_Registrador, "AX") == 0 || strcmp(Nome_Registrador, "AH") == 0 || strcmp(Nome_Registrador, "AL") == 0) {
		reg = 3;
	}
	if (strcmp(Nome_Registrador, "BX") == 0 || strcmp(Nome_Registrador, "BH") == 0 || strcmp(Nome_Registrador, "BL") == 0) {
		reg = 2;
	}
	if (strcmp(Nome_Registrador, "CX") == 0 || strcmp(Nome_Registrador, "CH") == 0 || strcmp(Nome_Registrador, "CL") == 0) {
		reg = 1;
	}
	if (strcmp(Nome_Registrador, "DX") == 0 || strcmp(Nome_Registrador, "DH") == 0 || strcmp(Nome_Registrador, "DL") == 0) {
		reg = 0;
	}
	return reg;
}

/**
	Mostra a simulação graficalmente
	Origem: Mostra de onde está no nivel de hardware a simulação
	Destino: Mostra onde está indo no nivel de hardware a simulação
	Origem/Destino são:
	0: Memoria, 1: BIO, 2: Queue, 3: Control Timer, 4: IP, 5: Inc IP, 6: Registrador, 7: ULA, 8: Z0
	Registrador: Qual registrador vai receber/enviar o dado:
	0 : DX,DL,DH  1: CX,CL,CJ 2: BX,BH,BL 3: AX,BH,BL
	Tx: Qual entrade da Ula vai receber os dado
	1: T1, 2: T2, 3: Instrução aplicada na ula
	Tx também pode assumir o valor de um registrador caso esteja movendo um registrador para registrador,
	 neste caso o valor de Tx será equivalente ao de Registrador

	 String1 e 2 são informações que você deseja informar na simulação
*/
void Dump_Simulacao(int Origem,int Destino, int Registrador, int Tx, char *String1, char *String2) {
	MT2D_Clear_Main_Window();
	strings = String1;
	strgings2 = String2;
	Desenha_Estrutura_Hardware();
	MT2D_Draw_Window(DISPLAY_WINDOW1);
	MT2D_System_Delay(20 * Velocidade_Simulador);
	switch(Origem)
	{
	case 0: //memória
		Move_Memoria_Bio(); //só da para ir para a bio
		break;
	case 1: //BIO
		switch (Destino) {
		case 0: // BIO Memoria
			Move_Bio_Memoria();
			break;
		case 2: // BIO Queue
			Move_Bio_Queue();
			break;
		case 6: // BIO Registrador
			Move_Bio_Registrador_Inicio();
			Move_Registrador_Inicio_Registrador(Registrador);
			break;
		case 7: // BIO ULA
			Move_Bio_ULA_Inicio();
			Move_ULA_Inicio_Tx(Tx);
		break;
		default:
			MT2D_MessageBox("Destino Invalido para BIO");
		}
		break;
	case 2: // Queue 
		switch (Destino) {
		case 1:
			Move_Queue_Bio(); //falta implementar
			break;
		case 3: // Control Time Decode
			Move_Queue_Controle();
			break;
		case 6: // Queue Registrador
			Move_Queue_Registrador_Inicio();
			Move_Registrador_Inicio_Registrador(Registrador);
			break;
		case 7: // Queue ULA
			Move_Queue_ULA_Inicio();
			Move_ULA_Inicio_Tx(Tx);
			break;
		default:
			MT2D_MessageBox("Destino Invalido para Queue");
			break;
		}
		break;
	case 3: // Control Timer
		break; //não vai pra algum lugar?
	case 4: // IP
		switch (Destino) {
		case 1: // IP BIU
			Move_IP_BIO();
			break;
		default:
			MT2D_MessageBox("Destino Invalido para Queue");
			break;
		}	
		break;

	case 5: // INC
		switch (Destino) {
		case 4: // INC IP
			Move_INC_IP();
			break;
		default:
			MT2D_MessageBox("Destino Invalido para INC");
			break;
		}
		break;
	case 6: // Registrador
		switch (Destino) {
		case 1:// Registrador BIO
			Move_Registrador_Bio(Registrador);
			break;
		case 2:// Registrador Queue
			Move_Registrador_Queue(Registrador);
			break;
		case 6://Registrador Registrador
			Move_Registrador_Registrador_Inicio(Registrador);
			Move_Registrador_Inicio_Registrador(Tx);
			break;
		case 7://Registrador ULA
			Move_Registrador_ULA_Inicio(Registrador);
			Move_ULA_Inicio_Tx(Tx);
			break;
		default:
			MT2D_MessageBox("Destino Invalido para Registrador");
			break;
		}
		break;
		case 7://ULA Z0
			Move_ULA_Zo();
			switch (Destino) {
			case 1: // ULA BIO
				Move_ULA_Bio();
				break;
			case 2: // ULA QUEUE
				Move_ULA_Queue();
				break;
			case 6: // ULA Registrador
				Move_ULA_Registrador_Inicio();
				Move_Registrador_Inicio_Registrador(Registrador);
				break;
			default:
				MT2D_MessageBox("Destino Invalido para ULA");
			}
		break;
	}
	if (MT2D_Keyboard_touched()) {
		if (MT2D_Keyboard_keytouched() == arrow_key_pressed) {
			MT2D_Keyboard_keytouched();
		}
	}
	insert_string_on_display("Aperte qualquer tecla para proseguir", 9, 0, DISPLAY_WINDOW1);
	MT2D_Draw_Window(DISPLAY_WINDOW1);
	if (MT2D_Keyboard_keytouched() == arrow_key_pressed) {
		MT2D_Keyboard_keytouched();
	}
	Menu_Simulador();
}


void Desenha_Estrutura_Hardware() {
	insert_string_on_display(strings, 0, 0, DISPLAY_WINDOW1);
	insert_string_on_display(strgings2, 1, 0, DISPLAY_WINDOW1);

	int X = 0, Y = 0;
//desenha a memória
	for (X = 74; X < 80; X++) {
		for (Y = 1; Y < 25; Y++) {
			WINDOW1[Y][X] = 176;
		}
	}
	insert_string_on_display("MEMORIA", 0, 73, DISPLAY_WINDOW1);
//desenha BIU
	for (X = 67; X < 70; X++) {
		for (Y = 1; Y < 25; Y++) {
			WINDOW1[Y][X] = 176;
		}
	}
	insert_string_on_display("BIU", 0, 67, DISPLAY_WINDOW1);

//desenha cache
	insert_string_on_display("CACHE", 0, 52, DISPLAY_WINDOW1);
	for (X = 50; X < 65; X++) {
		for (Y = 1 ; Y < 4 ; Y++) {
			WINDOW1[Y][X] = 176;
		}
	}

//desenha ligação de memória com a BIU
	Y = 8;
	for (X = 70; X < 74; X++) {
		WINDOW1[Y][X] = 196;
	}
	WINDOW1[8][70] = 195;
	WINDOW1[8][73] = 180;
//desenha QUEUE
	for (X = 48; X < 64; X++) {
		for (Y = 5; Y < 11; Y++) {
			WINDOW1[Y][X] = 177;
		}
	}
	insert_string_on_display("QUEUE", 11, 49, DISPLAY_WINDOW1);
//desenha DECODE E TIMING E CONTROL
	for (X = 49; X < 64; X++) {
		for (Y = 13; Y < 17; Y++) {
			WINDOW1[Y][X] = 177;
		}
	}
	insert_string_on_display("DEC TIM CONTROL", 17, 49, DISPLAY_WINDOW1);
//desenha ligacao de QUEUE com DECODE E TIMING E CONTROL
	WINDOW1[10][56] = 179;
	WINDOW1[11][56] = 179;
	WINDOW1[12][56] = 193;

//desenha registradores
	for (X = 23; X < 27; X++) {
		for (Y = 4; Y < 7; Y++) {
			WINDOW1[Y][X] = 176;
		}
	}
	insert_string_on_display("AX",5, 24, DISPLAY_WINDOW1);
	for (X = 27; X < 31; X++) {
		for (Y = 4; Y < 7; Y++) {
			WINDOW1[Y][X] = 177;
		}
	}
	insert_string_on_display("BX", 5,28,DISPLAY_WINDOW1);
	for (X = 31; X < 35; X++) {
		for (Y = 4; Y < 7; Y++) {
			WINDOW1[Y][X] = 178;
		}
	}
	insert_string_on_display("CX",5, 32, DISPLAY_WINDOW1);
	for (X = 35; X < 39; X++) {
		for (Y = 4; Y < 7; Y++) {
			WINDOW1[Y][X] = 176;
		}
	}
	insert_string_on_display("DX", 5,36, DISPLAY_WINDOW1);
// desenha  ULA
	for (X = 26; X < 30; X++) {
		for (Y = 14; Y < 17; Y++) {
			WINDOW1[Y][X] = 177;
		}
	}
	insert_string_on_display("T1", 15,27, DISPLAY_WINDOW1);

	for (X = 35; X < 39; X++) {
		for (Y = 14; Y < 17; Y++) {
			WINDOW1[Y][X] = 177;
		}
	}
	insert_string_on_display("T2", 15,36, DISPLAY_WINDOW1);

	for (X = 26; X < 39; X++) {
		for (Y = 17; Y < 21; Y++) {
			WINDOW1[Y][X] = 178;
		}
	}
	insert_string_on_display("ULA", 21, 35, DISPLAY_WINDOW1);

	for (X = 30; X < 34; X++) {
		for (Y = 22; Y < 25; Y++) {
			WINDOW1[Y][X] = 176;
		}
	}
	insert_string_on_display("Z0", 23,31, DISPLAY_WINDOW1);

//Desenha ligações finais.
	//Registrador
	for (X = 23; X < 41; X++) {
		WINDOW1[2][X] = 196;
		WINDOW1[8][X] = 196;
	}
	for (X = 25; X < 41; X+=4) {
		WINDOW1[2][X] = 194;
		WINDOW1[3][X] = 193;
		WINDOW1[7][X] = 194;
		WINDOW1[8][X] = 193;
	}
	WINDOW1[2][40] = 191;
	WINDOW1[8][40] = 217;

	// Queue BIU registrador
	WINDOW1[2][42] = 218;
	for (X = 43; X < 66; X ++) {
		WINDOW1[2][X] = 196;
	}
	WINDOW1[2][66] = 180;
	WINDOW1[2][56] = 194;
	WINDOW1[3][56] = 193;
	for (Y = 3; Y < 8; Y++) {
		WINDOW1[Y][40] = 179;
		WINDOW1[Y][42] = 179;
	}
	WINDOW1[9][42] = 179;
	WINDOW1[5][41] = 196;
	WINDOW1[5][40] = 195;
	WINDOW1[5][42] = 180;
	// Registrador ULA
	WINDOW1[8][42] = 179;
	WINDOW1[10][42] = 217;
	WINDOW1[10][32] = 218;
	for (X = 33; X < 42; X++) {
		WINDOW1[10][X] = 196;
	}
	WINDOW1[11][32] = 179;
	for (X = 25; X < 36; X++) {
		WINDOW1[12][X] = 196;
	}
	WINDOW1[12][24] = 218;
	WINDOW1[12][28] = 194;
	WINDOW1[12][32] = 197;
	WINDOW1[12][36] = 191;
	WINDOW1[13][28] = 193;
	WINDOW1[13][36] = 193;

	WINDOW1[16][32] = 193;
	for (Y = 13; Y < 16; Y++) {
		WINDOW1[Y][24] = 179;
		WINDOW1[Y][32] = 179;
	}
	for (Y = 16; Y < 23; Y++) {
		WINDOW1[Y][24] = 179;
	}
	WINDOW1[23][24] = 192;
	for (X = 25; X < 29; X++) {
		WINDOW1[23][X] = 196;
	}
	WINDOW1[23][29] = 180;
	WINDOW1[21][32] = 186;
//	Desenha IP
	for (X = 59; X < 63; X++) {
		for (Y = 21; Y < 24; Y++) {
			WINDOW1[Y][X] = 176;
		}
	}
	insert_string_on_display("IP", 22, 60, DISPLAY_WINDOW1);

	for (X = 51; X < 55; X++) {
		for (Y = 21; Y < 24; Y++) {
			WINDOW1[Y][X] = 176;
		}
	}
	insert_string_on_display("INC", 22, 51, DISPLAY_WINDOW1);
	for (X = 1; X < 3; X++) {
		WINDOW1[22][X + 55] = 196;
		WINDOW1[22][X + 63] = 196;

	}
	WINDOW1[22][55] = 195;
	WINDOW1[22][58] = 196;
	WINDOW1[22][63] = 196;
	WINDOW1[22][66] = 180;

	WINDOW1[20][53] = 179;
	WINDOW1[20][60] = 193;

	for (X = 54; X < 60; X++) {
		WINDOW1[19][X] = 196;
	}
	WINDOW1[19][53] = 218;
	WINDOW1[19][60] = 191;
}

void Move_Memoria_Bio() {
	int X = 73, Y = 8;
	while (X > 69) {
		MT2D_Clear_Main_Window();
		Desenha_Estrutura_Hardware();
		WINDOW1[Y][X] = '*';
		MT2D_Draw_Window(DISPLAY_WINDOW1);
		MT2D_System_Delay(Velocidade_Simulador);
		X--;
	}
}
void Move_Bio_Memoria() {
	int X = 69, Y = 8;
	while (X < 74) {
		MT2D_Clear_Main_Window();
		Desenha_Estrutura_Hardware();
		WINDOW1[Y][X] = '*';
		MT2D_Draw_Window(DISPLAY_WINDOW1);
		MT2D_System_Delay(Velocidade_Simulador);
		X++;
	}
}
void Move_Bio_Queue() {
	int X = 66, Y = 2;
	while (X > 55) {
		MT2D_Clear_Main_Window();
		Desenha_Estrutura_Hardware();
		WINDOW1[Y][X] = '*';
		MT2D_Draw_Window(DISPLAY_WINDOW1);
		MT2D_System_Delay(Velocidade_Simulador);
		X--;
	}
	MT2D_Clear_Main_Window();
	Desenha_Estrutura_Hardware();
	WINDOW1[3][56] = '*';
	MT2D_Draw_Window(DISPLAY_WINDOW1);
}
void Move_Bio_Registrador_Inicio() {
	int X = 66, Y = 2;
	while (X > 41) {
		MT2D_Clear_Main_Window();
		Desenha_Estrutura_Hardware();
		WINDOW1[Y][X] = '*';
		MT2D_Draw_Window(DISPLAY_WINDOW1);
		MT2D_System_Delay(Velocidade_Simulador);
		X--;
	}
	X++;
	while (Y < 6) {
		MT2D_Clear_Main_Window();
		Desenha_Estrutura_Hardware();
		WINDOW1[Y][X] = '*';
		MT2D_Draw_Window(DISPLAY_WINDOW1);
		MT2D_System_Delay(Velocidade_Simulador);
		Y++;
	}
	Y--;
	while (X > 39) {
		MT2D_Clear_Main_Window();
		Desenha_Estrutura_Hardware();
		WINDOW1[Y][X] = '*';
		MT2D_Draw_Window(DISPLAY_WINDOW1);
		MT2D_System_Delay(Velocidade_Simulador);
		X--;
	}
	X++;
	while (Y >1) {
		MT2D_Clear_Main_Window();
		Desenha_Estrutura_Hardware();
		WINDOW1[Y][X] = '*';
		MT2D_Draw_Window(DISPLAY_WINDOW1);
		MT2D_System_Delay(Velocidade_Simulador);
		Y--;
	}

}
void Move_Bio_ULA_Inicio() {
	int X = 66, Y = 2;
	while (X > 41) {
		MT2D_Clear_Main_Window();
		Desenha_Estrutura_Hardware();
		WINDOW1[Y][X] = '*';
		MT2D_Draw_Window(DISPLAY_WINDOW1);
		MT2D_System_Delay(Velocidade_Simulador);
		X--;
	}
	X++;
	while (Y < 11) {
		MT2D_Clear_Main_Window();
		Desenha_Estrutura_Hardware();
		WINDOW1[Y][X] = '*';
		MT2D_Draw_Window(DISPLAY_WINDOW1);
		MT2D_System_Delay(Velocidade_Simulador);
		Y++;
	}
	Y--;
	while (X > 31) {
		MT2D_Clear_Main_Window();
		Desenha_Estrutura_Hardware();
		WINDOW1[Y][X] = '*';
		MT2D_Draw_Window(DISPLAY_WINDOW1);
		MT2D_System_Delay(Velocidade_Simulador);
		X--;
	}
	X++;
	while (Y < 13) {
		MT2D_Clear_Main_Window();
		Desenha_Estrutura_Hardware();
		WINDOW1[Y][X] = '*';
		MT2D_Draw_Window(DISPLAY_WINDOW1);
		MT2D_System_Delay(Velocidade_Simulador);
		Y++;
	}
	Y--;
}
void Move_Queue_Controle() {
	int X=56, Y=10;
	while (Y < 13) {
		MT2D_Clear_Main_Window();
		Desenha_Estrutura_Hardware();
		WINDOW1[Y][X] = '*';
		MT2D_Draw_Window(DISPLAY_WINDOW1);
		MT2D_System_Delay(Velocidade_Simulador);
		Y++;
	}
}

void Move_Queue_Bio() {
	int X = 56, Y = 2;
	while (X < 67) {
		MT2D_Clear_Main_Window();
		Desenha_Estrutura_Hardware();
		WINDOW1[Y][X] = '*';
		MT2D_Draw_Window(DISPLAY_WINDOW1);
		MT2D_System_Delay(Velocidade_Simulador);
		X++;
	}
	X++;

}

void Move_Queue_Registrador_Inicio() {
	int X = 56, Y = 2;
	while (X > 41) {
		MT2D_Clear_Main_Window();
		Desenha_Estrutura_Hardware();
		WINDOW1[Y][X] = '*';
		MT2D_Draw_Window(DISPLAY_WINDOW1);
		MT2D_System_Delay(Velocidade_Simulador);
		X--;
	}
	X++;
	while (Y < 6) {
		MT2D_Clear_Main_Window();
		Desenha_Estrutura_Hardware();
		WINDOW1[Y][X] = '*';
		MT2D_Draw_Window(DISPLAY_WINDOW1);
		MT2D_System_Delay(Velocidade_Simulador);
		Y++;
	}
	Y--;
	while (X > 39) {
		MT2D_Clear_Main_Window();
		Desenha_Estrutura_Hardware();
		WINDOW1[Y][X] = '*';
		MT2D_Draw_Window(DISPLAY_WINDOW1);
		MT2D_System_Delay(Velocidade_Simulador);
		X--;
	}
	X++;
	while (Y >1) {
		MT2D_Clear_Main_Window();
		Desenha_Estrutura_Hardware();
		WINDOW1[Y][X] = '*';
		MT2D_Draw_Window(DISPLAY_WINDOW1);
		MT2D_System_Delay(Velocidade_Simulador);
		Y--;
	}

}
void Move_Queue_ULA_Inicio() {
	int X = 56, Y = 2;
	while (X > 41) {
		MT2D_Clear_Main_Window();
		Desenha_Estrutura_Hardware();
		WINDOW1[Y][X] = '*';
		MT2D_Draw_Window(DISPLAY_WINDOW1);
		MT2D_System_Delay(Velocidade_Simulador);
		X--;
	}
	X++;
	while (Y < 11) {
		MT2D_Clear_Main_Window();
		Desenha_Estrutura_Hardware();
		WINDOW1[Y][X] = '*';
		MT2D_Draw_Window(DISPLAY_WINDOW1);
		MT2D_System_Delay(Velocidade_Simulador);
		Y++;
	}
	Y--;
	while (X > 31) {
		MT2D_Clear_Main_Window();
		Desenha_Estrutura_Hardware();
		WINDOW1[Y][X] = '*';
		MT2D_Draw_Window(DISPLAY_WINDOW1);
		MT2D_System_Delay(Velocidade_Simulador);
		X--;
	}
	X++;
	while (Y < 13) {
		MT2D_Clear_Main_Window();
		Desenha_Estrutura_Hardware();
		WINDOW1[Y][X] = '*';
		MT2D_Draw_Window(DISPLAY_WINDOW1);
		MT2D_System_Delay(Velocidade_Simulador);
		Y++;
	}
	Y--;

}
void Move_IP_BIO() {
	int X = 63, Y = 22;
	while (X < 67) {
		MT2D_Clear_Main_Window();
		Desenha_Estrutura_Hardware();
		WINDOW1[Y][X] = '*';
		MT2D_Draw_Window(DISPLAY_WINDOW1);
		MT2D_System_Delay(Velocidade_Simulador);
		X++;
	}
}
void Move_INC_IP() {
	int X = 53, Y = 20;
	MT2D_Clear_Main_Window();
	Desenha_Estrutura_Hardware();
	WINDOW1[Y][X] = '*';
	MT2D_Draw_Window(DISPLAY_WINDOW1);
	MT2D_System_Delay(Velocidade_Simulador);
	Y--;
	while (X < 61) {
		MT2D_Clear_Main_Window();
		Desenha_Estrutura_Hardware();
		WINDOW1[Y][X] = '*';
		MT2D_Draw_Window(DISPLAY_WINDOW1);
		MT2D_System_Delay(Velocidade_Simulador);
		X++;
	}
	MT2D_Clear_Main_Window();
	Desenha_Estrutura_Hardware();
	WINDOW1[20][60] = '*';
	MT2D_Draw_Window(DISPLAY_WINDOW1);
	MT2D_System_Delay(Velocidade_Simulador);

}
void Move_Registrador_Inicio_Registrador(int Registrador) {
	int tempo_delay = 700 - 100 * Registrador;
	int X = 40, Y = 2;
	int fim = (Registrador + 1) * 4;
	int cnt = 0;
	while (X > 36) {
		MT2D_Clear_Main_Window();
		Desenha_Estrutura_Hardware();
		WINDOW1[Y][X] = '*';
		MT2D_Draw_Window(DISPLAY_WINDOW1);
		MT2D_System_Delay(Velocidade_Simulador);
		X--;
	}
	X++;
	while (Registrador > 0) {
		cnt = 0;
		while (cnt  < 4) {
			MT2D_Clear_Main_Window();
			Desenha_Estrutura_Hardware();
			WINDOW1[Y][X] = '*';
			MT2D_Draw_Window(DISPLAY_WINDOW1);
			MT2D_System_Delay(Velocidade_Simulador);
			X--;
			cnt++;
		}
		Registrador--;
	}
	MT2D_System_Delay(Velocidade_Simulador);
	Y++;
	MT2D_Clear_Main_Window();
	Desenha_Estrutura_Hardware();
	WINDOW1[Y][X] = '*';
	MT2D_Draw_Window(DISPLAY_WINDOW1);
	MT2D_System_Delay(Velocidade_Simulador);
}
void Move_Registrador_ULA_Inicio(int Registrador) {
	int X = 37 - 4*Registrador, Y=7;
	// MOVE PARA A SAIDA DO INICIO DOS REGISTRADORES
	MT2D_Clear_Main_Window();
	Desenha_Estrutura_Hardware();
	WINDOW1[Y][X] = '*';
	MT2D_Draw_Window(DISPLAY_WINDOW1);
	MT2D_System_Delay(Velocidade_Simulador);
	Y++;
	while (X < 41) {
		MT2D_Clear_Main_Window();
		Desenha_Estrutura_Hardware();
		WINDOW1[Y][X] = '*';
		MT2D_Draw_Window(DISPLAY_WINDOW1);
		MT2D_System_Delay(Velocidade_Simulador);
		X++;
	}
	X--;
	while (Y > 4) {
		MT2D_Clear_Main_Window();
		Desenha_Estrutura_Hardware();
		WINDOW1[Y][X] = '*';
		MT2D_Draw_Window(DISPLAY_WINDOW1);
		MT2D_System_Delay(Velocidade_Simulador);
		Y--;
	}
	Y++;
	while (X < 43) {
		MT2D_Clear_Main_Window();
		Desenha_Estrutura_Hardware();
		WINDOW1[Y][X] = '*';
		MT2D_Draw_Window(DISPLAY_WINDOW1);
		MT2D_System_Delay(Velocidade_Simulador);
		X++;
	}
	X--;
	//FIM SAIDA
	while (Y < 11) {
		MT2D_Clear_Main_Window();
		Desenha_Estrutura_Hardware();
		WINDOW1[Y][X] = '*';
		MT2D_Draw_Window(DISPLAY_WINDOW1);
		MT2D_System_Delay(Velocidade_Simulador);
		Y++;
	}
	Y--;
	while (X > 31) {
		MT2D_Clear_Main_Window();
		Desenha_Estrutura_Hardware();
		WINDOW1[Y][X] = '*';
		MT2D_Draw_Window(DISPLAY_WINDOW1);
		MT2D_System_Delay(Velocidade_Simulador);
		X--;
	}
	X++;
	while (Y < 13) {
		MT2D_Clear_Main_Window();
		Desenha_Estrutura_Hardware();
		WINDOW1[Y][X] = '*';
		MT2D_Draw_Window(DISPLAY_WINDOW1);
		MT2D_System_Delay(Velocidade_Simulador);
		Y++;
	}
}
void Move_Registrador_Queue(int Registrador) {
	int X = 37 - 4 * Registrador, Y = 7;
	// MOVE PARA A SAIDA DO INICIO DOS REGISTRADORES
	MT2D_Clear_Main_Window();
	Desenha_Estrutura_Hardware();
	WINDOW1[Y][X] = '*';
	MT2D_Draw_Window(DISPLAY_WINDOW1);
	MT2D_System_Delay(Velocidade_Simulador);
	Y++;
	while (X < 41) {
		MT2D_Clear_Main_Window();
		Desenha_Estrutura_Hardware();
		WINDOW1[Y][X] = '*';
		MT2D_Draw_Window(DISPLAY_WINDOW1);
		MT2D_System_Delay(Velocidade_Simulador);
		X++;
	}
	X--;
	while (Y > 4) {
		MT2D_Clear_Main_Window();
		Desenha_Estrutura_Hardware();
		WINDOW1[Y][X] = '*';
		MT2D_Draw_Window(DISPLAY_WINDOW1);
		MT2D_System_Delay(Velocidade_Simulador);
		Y--;
	}
	Y++;
	while (X < 43) {
		MT2D_Clear_Main_Window();
		Desenha_Estrutura_Hardware();
		WINDOW1[Y][X] = '*';
		MT2D_Draw_Window(DISPLAY_WINDOW1);
		MT2D_System_Delay(Velocidade_Simulador);
		X++;
	}
	X--;
	//FIM SAIDA
	while (Y > 1) {
		MT2D_Clear_Main_Window();
		Desenha_Estrutura_Hardware();
		WINDOW1[Y][X] = '*';
		MT2D_Draw_Window(DISPLAY_WINDOW1);
		MT2D_System_Delay(Velocidade_Simulador);
		Y--;
	}
	Y++;
	while (X < 57) {
		MT2D_Clear_Main_Window();
		Desenha_Estrutura_Hardware();
		WINDOW1[Y][X] = '*';
		MT2D_Draw_Window(DISPLAY_WINDOW1);
		MT2D_System_Delay(Velocidade_Simulador);
		X++;
	}
	X--;
	Y++;
	MT2D_Clear_Main_Window();
	Desenha_Estrutura_Hardware();
	WINDOW1[Y][X] = '*';
	MT2D_Draw_Window(DISPLAY_WINDOW1);
	MT2D_System_Delay(Velocidade_Simulador);
}
void Move_Registrador_Bio(int Registrador) {
	int X = 37 - 4 * Registrador, Y = 7;
	// MOVE PARA A SAIDA DO INICIO DOS REGISTRADORES
	MT2D_Clear_Main_Window();
	Desenha_Estrutura_Hardware();
	WINDOW1[Y][X] = '*';
	MT2D_Draw_Window(DISPLAY_WINDOW1);
	MT2D_System_Delay(Velocidade_Simulador);
	Y++;
	while (X < 41) {
		MT2D_Clear_Main_Window();
		Desenha_Estrutura_Hardware();
		WINDOW1[Y][X] = '*';
		MT2D_Draw_Window(DISPLAY_WINDOW1);
		MT2D_System_Delay(Velocidade_Simulador);
		X++;
	}
	X--;
	while (Y > 4) {
		MT2D_Clear_Main_Window();
		Desenha_Estrutura_Hardware();
		WINDOW1[Y][X] = '*';
		MT2D_Draw_Window(DISPLAY_WINDOW1);
		MT2D_System_Delay(Velocidade_Simulador);
		Y--;
	}
	Y++;
	while (X < 43) {
		MT2D_Clear_Main_Window();
		Desenha_Estrutura_Hardware();
		WINDOW1[Y][X] = '*';
		MT2D_Draw_Window(DISPLAY_WINDOW1);
		MT2D_System_Delay(Velocidade_Simulador);
		X++;
	}
	X--;
	//FIM SAIDA
	while (Y > 1) {
		MT2D_Clear_Main_Window();
		Desenha_Estrutura_Hardware();
		WINDOW1[Y][X] = '*';
		MT2D_Draw_Window(DISPLAY_WINDOW1);
		MT2D_System_Delay(Velocidade_Simulador);
		Y--;
	}
	Y++;
	while (X < 67) {
		MT2D_Clear_Main_Window();
		Desenha_Estrutura_Hardware();
		WINDOW1[Y][X] = '*';
		MT2D_Draw_Window(DISPLAY_WINDOW1);
		MT2D_System_Delay(Velocidade_Simulador);
		X++;
	}
}

void Move_Registrador_Registrador_Inicio(int Registrador) {
	int X = 37 - 4 * Registrador, Y = 7;
	MT2D_Clear_Main_Window();
	Desenha_Estrutura_Hardware();
	WINDOW1[Y][X] = '*';
	MT2D_Draw_Window(DISPLAY_WINDOW1);
	MT2D_System_Delay(Velocidade_Simulador);
	Y++;
	while (X < 41) {
		MT2D_Clear_Main_Window();
		Desenha_Estrutura_Hardware();
		WINDOW1[Y][X] = '*';
		MT2D_Draw_Window(DISPLAY_WINDOW1);
		MT2D_System_Delay(Velocidade_Simulador);
		X++;
	}
	X--;
	while (Y > 1) {
		MT2D_Clear_Main_Window();
		Desenha_Estrutura_Hardware();
		WINDOW1[Y][X] = '*';
		MT2D_Draw_Window(DISPLAY_WINDOW1);
		MT2D_System_Delay(Velocidade_Simulador);
		Y--;
	}
	Y++;
}
void Move_ULA_Inicio_Tx(int Tx) {
	int X=32, Y=12;
	if (Tx == 1) {
		while (X > 27) {
			MT2D_Clear_Main_Window();
			Desenha_Estrutura_Hardware();
			WINDOW1[Y][X] = '*';
			MT2D_Draw_Window(DISPLAY_WINDOW1);
			MT2D_System_Delay(Velocidade_Simulador);
			X--;
		}
		X++;
	}
	else if(Tx == 2) {
		while (X < 37) {
			MT2D_Clear_Main_Window();
			Desenha_Estrutura_Hardware();
			WINDOW1[Y][X] = '*';
			MT2D_Draw_Window(DISPLAY_WINDOW1);
			MT2D_System_Delay(Velocidade_Simulador);
			X++;
		}
		X--;
	}
	else {
		while (Y < 16) {
			MT2D_Clear_Main_Window();
			Desenha_Estrutura_Hardware();
			WINDOW1[Y][X] = '*';
			MT2D_Draw_Window(DISPLAY_WINDOW1);
			MT2D_System_Delay(Velocidade_Simulador);
			Y++;
		}
		Y--;
	}
	Y++;
	MT2D_Clear_Main_Window();
	Desenha_Estrutura_Hardware();
	WINDOW1[Y][X] = '*';
	MT2D_Draw_Window(DISPLAY_WINDOW1);
	MT2D_System_Delay(Velocidade_Simulador);

}
void Move_ULA_Zo() {
	MT2D_Clear_Main_Window();
	Desenha_Estrutura_Hardware();
	WINDOW1[21][32] = '*';
	MT2D_Draw_Window(DISPLAY_WINDOW1);
	MT2D_System_Delay(Velocidade_Simulador * 20);
}

void Move_Z0_ULA_Inicio() {
	int X = 29, Y = 23;
	while (X > 23) {
		MT2D_Clear_Main_Window();
		Desenha_Estrutura_Hardware();
		WINDOW1[Y][X] = '*';
		MT2D_Draw_Window(DISPLAY_WINDOW1);
		MT2D_System_Delay(Velocidade_Simulador);
		X--;
	}
	X++;
	while (Y > 11) {
		MT2D_Clear_Main_Window();
		Desenha_Estrutura_Hardware();
		WINDOW1[Y][X] = '*';
		MT2D_Draw_Window(DISPLAY_WINDOW1);
		MT2D_System_Delay(Velocidade_Simulador);
		Y--;
	}
	Y++;
	while (X < 33) {
		MT2D_Clear_Main_Window();
		Desenha_Estrutura_Hardware();
		WINDOW1[Y][X] = '*';
		MT2D_Draw_Window(DISPLAY_WINDOW1);
		MT2D_System_Delay(Velocidade_Simulador);
		X++;
	}

}

void Move_ULA_Registrador_Inicio() {
	Move_Z0_ULA_Inicio();
	int X=32, Y=12;
	while (Y > 9) {
		MT2D_Clear_Main_Window();
		Desenha_Estrutura_Hardware();
		WINDOW1[Y][X] = '*';
		MT2D_Draw_Window(DISPLAY_WINDOW1);
		MT2D_System_Delay(Velocidade_Simulador);
		Y--;
	}
	Y++;
	while (X < 43) {
		MT2D_Clear_Main_Window();
		Desenha_Estrutura_Hardware();
		WINDOW1[Y][X] = '*';
		MT2D_Draw_Window(DISPLAY_WINDOW1);
		MT2D_System_Delay(Velocidade_Simulador);
		X++;
	}
	X--;
	while (Y > 4) {
		MT2D_Clear_Main_Window();
		Desenha_Estrutura_Hardware();
		WINDOW1[Y][X] = '*';
		MT2D_Draw_Window(DISPLAY_WINDOW1);
		MT2D_System_Delay(Velocidade_Simulador);
		Y--;
	}
	Y++;
	while (X > 39) {
		MT2D_Clear_Main_Window();
		Desenha_Estrutura_Hardware();
		WINDOW1[Y][X] = '*';
		MT2D_Draw_Window(DISPLAY_WINDOW1);
		MT2D_System_Delay(Velocidade_Simulador);
		X--;
	}
	X++;
	while (Y > 1) {
		MT2D_Clear_Main_Window();
		Desenha_Estrutura_Hardware();
		WINDOW1[Y][X] = '*';
		MT2D_Draw_Window(DISPLAY_WINDOW1);
		MT2D_System_Delay(Velocidade_Simulador);
		Y--;
	}
	Y++;
}
void Move_ULA_Queue() {
	Move_Z0_ULA_Inicio();
	int X = 32, Y = 12;
	while (Y > 9) {
		MT2D_Clear_Main_Window();
		Desenha_Estrutura_Hardware();
		WINDOW1[Y][X] = '*';
		MT2D_Draw_Window(DISPLAY_WINDOW1);
		MT2D_System_Delay(Velocidade_Simulador);
		Y--;
	}
	Y++;
	while (X < 43) {
		MT2D_Clear_Main_Window();
		Desenha_Estrutura_Hardware();
		WINDOW1[Y][X] = '*';
		MT2D_Draw_Window(DISPLAY_WINDOW1);
		MT2D_System_Delay(Velocidade_Simulador);
		X++;
	}
	X--;
	while (Y > 1) {
		MT2D_Clear_Main_Window();
		Desenha_Estrutura_Hardware();
		WINDOW1[Y][X] = '*';
		MT2D_Draw_Window(DISPLAY_WINDOW1);
		MT2D_System_Delay(Velocidade_Simulador);
		Y--;
	}
	Y++;
	while (X < 57) {
		MT2D_Clear_Main_Window();
		Desenha_Estrutura_Hardware();
		WINDOW1[Y][X] = '*';
		MT2D_Draw_Window(DISPLAY_WINDOW1);
		MT2D_System_Delay(Velocidade_Simulador);
		X++;
	}
	X--;
	MT2D_Clear_Main_Window();
	Desenha_Estrutura_Hardware();
	WINDOW1[3][56] = '*';
	MT2D_Draw_Window(DISPLAY_WINDOW1);
	MT2D_System_Delay(Velocidade_Simulador);
}
void Move_ULA_Bio() {
	Move_Z0_ULA_Inicio();
	int X = 32, Y = 12;
	while (Y > 9) {
		MT2D_Clear_Main_Window();
		Desenha_Estrutura_Hardware();
		WINDOW1[Y][X] = '*';
		MT2D_Draw_Window(DISPLAY_WINDOW1);
		MT2D_System_Delay(Velocidade_Simulador);
		Y--;
	}
	Y++;
	while (X < 43) {
		MT2D_Clear_Main_Window();
		Desenha_Estrutura_Hardware();
		WINDOW1[Y][X] = '*';
		MT2D_Draw_Window(DISPLAY_WINDOW1);
		MT2D_System_Delay(Velocidade_Simulador);
		X++;
	}
	X--;
	while (Y > 1) {
		MT2D_Clear_Main_Window();
		Desenha_Estrutura_Hardware();
		WINDOW1[Y][X] = '*';
		MT2D_Draw_Window(DISPLAY_WINDOW1);
		MT2D_System_Delay(Velocidade_Simulador);
		Y--;
	}
	Y++;
	while (X < 67) {
		MT2D_Clear_Main_Window();
		Desenha_Estrutura_Hardware();
		WINDOW1[Y][X] = '*';
		MT2D_Draw_Window(DISPLAY_WINDOW1);
		MT2D_System_Delay(Velocidade_Simulador);
		X++;
	}
}