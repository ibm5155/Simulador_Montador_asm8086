#include "Texto.h"
#include "../../MT2D/Window_core.h"
#include "../../MT2D/Keyboard.h"
#include "CPU.h"
#include <stdlib.h>

extern unsigned char Memoria[65535];// 1 byte por char -> 64KB ram

void Dump_Registrador() {
	char *Dado = 0;
	int ID = 0;
	int Y = 2;
	int Saida = false;
	char *Booleano;
	do {
		clear_display();
		create_window_layout(0, 0, MAX_HOR - 1, MAX_VER - 1, false, false, 0, ' ', DISPLAY_WINDOW1);

#pragma region REGISTRADORES GERAL
		create_window_layout(3, 6, 22, 3, false, false, 0, ' ', DISPLAY_WINDOW1);
		create_window_layout(3, 9, 22 , 3, false, false, 0, ' ', DISPLAY_WINDOW1);
		create_window_layout(3, 12, 22 , 3, false, false, 0, ' ', DISPLAY_WINDOW1);
		create_window_layout(3, 15, 22 , 3, false, false, 0, ' ', DISPLAY_WINDOW1);

		insert_string_on_display("AX", 6, 13, DISPLAY_WINDOW1);
		insert_string_on_display("BX", 9, 13, DISPLAY_WINDOW1);
		insert_string_on_display("CX", 12, 13, DISPLAY_WINDOW1);
		insert_string_on_display("DX", 15, 13, DISPLAY_WINDOW1);

		insert_string_on_display("AH", 7, 4, DISPLAY_WINDOW1);
		insert_string_on_display("AL", 7, 14, DISPLAY_WINDOW1);
		ID = Registrador_PegaID(i8086.Registradores_Geral, "AH");
		Dado = InteiroParaCharBinario(i8086.Registradores_Geral->Registradores[ID].Dado,_8BITS);
		insert_string_on_display(Dado, 8, 4, DISPLAY_WINDOW1);
		free(Dado);
		ID = Registrador_PegaID(i8086.Registradores_Geral, "AL");
		Dado = InteiroParaCharBinario(i8086.Registradores_Geral->Registradores[ID].Dado, _8BITS);
		insert_string_on_display(Dado, 8, 14, DISPLAY_WINDOW1);
		free(Dado);


		insert_string_on_display("BH", 10, 4, DISPLAY_WINDOW1);
		insert_string_on_display("BL", 10, 14, DISPLAY_WINDOW1);
		ID = Registrador_PegaID(i8086.Registradores_Geral, "BH");
		Dado = InteiroParaCharBinario(i8086.Registradores_Geral->Registradores[ID].Dado, _8BITS);
		insert_string_on_display(Dado, 11, 4, DISPLAY_WINDOW1);
		free(Dado);
		ID = Registrador_PegaID(i8086.Registradores_Geral, "BL");
		Dado = InteiroParaCharBinario(i8086.Registradores_Geral->Registradores[ID].Dado, _8BITS);
		insert_string_on_display(Dado, 11, 14, DISPLAY_WINDOW1);
		free(Dado);

		insert_string_on_display("CH", 13, 4, DISPLAY_WINDOW1);
		insert_string_on_display("CL", 13, 14, DISPLAY_WINDOW1);
		ID = Registrador_PegaID(i8086.Registradores_Geral, "CH");
		Dado = InteiroParaCharBinario(i8086.Registradores_Geral->Registradores[ID].Dado, _8BITS);
		insert_string_on_display(Dado, 14, 4, DISPLAY_WINDOW1);
		free(Dado);
		ID = Registrador_PegaID(i8086.Registradores_Geral, "CL");
		Dado = InteiroParaCharBinario(i8086.Registradores_Geral->Registradores[ID].Dado, _8BITS);
		insert_string_on_display(Dado, 14, 14, DISPLAY_WINDOW1);
		free(Dado);

		insert_string_on_display("DH", 16, 4, DISPLAY_WINDOW1);
		insert_string_on_display("DL", 16, 14, DISPLAY_WINDOW1);
		ID = Registrador_PegaID(i8086.Registradores_Geral, "DH");
		Dado = InteiroParaCharBinario(i8086.Registradores_Geral->Registradores[ID].Dado, _8BITS);
		insert_string_on_display(Dado, 17, 4, DISPLAY_WINDOW1);
		free(Dado);
		ID = Registrador_PegaID(i8086.Registradores_Geral, "DL");
		Dado = InteiroParaCharBinario(i8086.Registradores_Geral->Registradores[ID].Dado, _8BITS);
		insert_string_on_display(Dado, 17, 14, DISPLAY_WINDOW1);
		free(Dado);
#pragma endregion 
#pragma region REGISTRADOR SEGMENTO
		create_window_layout(27, 6, 22, 3, false, false, 0, ' ', DISPLAY_WINDOW1);
		create_window_layout(27, 9, 22, 3, false, false, 0, ' ', DISPLAY_WINDOW1);
		create_window_layout(27, 12, 22, 3, false, false, 0, ' ', DISPLAY_WINDOW1);
		create_window_layout(27, 15, 22, 3, false, false, 0, ' ', DISPLAY_WINDOW1);

		insert_string_on_display("CS", 7, 28, DISPLAY_WINDOW1);
		insert_string_on_display("DS", 10, 28, DISPLAY_WINDOW1);
		insert_string_on_display("ES", 13, 28, DISPLAY_WINDOW1);
		insert_string_on_display("SS", 16, 28, DISPLAY_WINDOW1);

		ID = Registrador_PegaID(i8086.Registradores_Segmento, "CS");
		Dado = InteiroParaCharBinario(i8086.Registradores_Segmento->Registradores[ID].Dado, _16BITS);
		insert_string_on_display(Dado, 8, 28, DISPLAY_WINDOW1);
		free(Dado);

		ID = Registrador_PegaID(i8086.Registradores_Segmento, "DS");
		Dado = InteiroParaCharBinario(i8086.Registradores_Segmento->Registradores[ID].Dado, _16BITS);
		insert_string_on_display(Dado, 11, 28, DISPLAY_WINDOW1);
		free(Dado);

		ID = Registrador_PegaID(i8086.Registradores_Segmento, "ES");
		Dado = InteiroParaCharBinario(i8086.Registradores_Segmento->Registradores[ID].Dado, _16BITS);
		insert_string_on_display(Dado, 14, 28, DISPLAY_WINDOW1);
		free(Dado);

		ID = Registrador_PegaID(i8086.Registradores_Segmento, "SS");
		Dado = InteiroParaCharBinario(i8086.Registradores_Segmento->Registradores[ID].Dado, _16BITS);
		insert_string_on_display(Dado, 17, 28, DISPLAY_WINDOW1);
		free(Dado);
#pragma endregion


		insert_string_on_display("REGISTRADOR", 0, 10, DISPLAY_WINDOW1);
		print_display(DISPLAY_WINDOW1);
		Saida = Keyboard_keytouched();
		if (Saida == tab) {//ta hora de dizer tchau
		}
		else {
			Saida = false;
		}

	} while (!Saida);
}