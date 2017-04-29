#include "Texto.h"
#include <MT2D/MT2D.h>
#include <MT2D/MT2D_Display.h>
#include <MT2D/MT2D_Layout.h>
#include <MT2D/MT2D_Keyboard.h>
#include "CPU.h"
#include "RAM.h"
#include <stdlib.h>

void Dump_Registrador() {
	char *Dado = 0;
	int ID = 0;
	int Y = 2;
	int Saida = false;
	char *Booleano;
	do {
		MT2D_Clear_Main_Window();
		MT2D_Create_Window_Layout(0, 0, MAX_HOR - 1, MAX_VER - 1, false, false, 0, ' ', DISPLAY_WINDOW1); // cria uma "grid"

#pragma region REGISTRADORES GERAL

		for (int i = 0,j=0; j < i8086.Registradores_Geral->Quantidade - 4 /*Para ocultar os registradores depois de DX*/; j++) {
			if (i8086.Registradores_Geral->Registradores[j].Tamanho == _16BITS) {
				MT2D_Create_Window_Layout(3, 6 + (i * 3), 22, 3, false, false, 0, ' ', DISPLAY_WINDOW1); // cria uma "grid"
				insert_string_on_display(i8086.Registradores_Geral->Registradores[j].Nome, 6 + (3 * i), 13, DISPLAY_WINDOW1);

				if (i8086.Registradores_Geral->Registradores[j].High_offset == 0 && i8086.Registradores_Geral->Registradores[j].Low_offset == 0) {
					Dado = InteiroParaCharBinario(i8086.Registradores_Geral->Registradores[ID].Dado, _16BITS);
					insert_string_on_display(Dado, 8 + (3 * i), 4, DISPLAY_WINDOW1);
					free(Dado);
					i++;
				}
				else {

					ID = i8086.Registradores_Geral->Registradores[j].High_offset;
					insert_string_on_display(i8086.Registradores_Geral->Registradores[ID].Nome, 7 + (3 * i), 4, DISPLAY_WINDOW1);
					Dado = InteiroParaCharBinario(i8086.Registradores_Geral->Registradores[ID].Dado, _8BITS);
					insert_string_on_display(Dado, 8 + (3 * i), 4, DISPLAY_WINDOW1);
					free(Dado);


					ID = i8086.Registradores_Geral->Registradores[j].Low_offset;
					insert_string_on_display(i8086.Registradores_Geral->Registradores[ID].Nome, 7 + (3 * i), 14, DISPLAY_WINDOW1);
					Dado = InteiroParaCharBinario(i8086.Registradores_Geral->Registradores[ID].Dado, _8BITS);
					insert_string_on_display(Dado, 8 + (3 * i), 14, DISPLAY_WINDOW1);
					free(Dado);
					i++;
				}
			}
		}
#pragma endregion 
#pragma region ULA
		for (int i = 0; i < 3; i++) {
			MT2D_Create_Window_Layout(27, 6 + (3 * i), 22, 3, false, false, 0, ' ', DISPLAY_WINDOW1); // cria uma "grid"
			if (i == 0) {
				insert_string_on_display("T1", 7 + (3 * i), 28, DISPLAY_WINDOW1);
				Dado = InteiroParaCharBinario(i8086.ULA_T1, _16BITS);
			}
			else if (i == 1) {
				insert_string_on_display("T2", 7 + (3 * i), 28, DISPLAY_WINDOW1);
				Dado = InteiroParaCharBinario(i8086.ULA_T2, _16BITS);
			}else if (i == 2) {
				insert_string_on_display("OUT", 7 + (3 * i), 28, DISPLAY_WINDOW1);
				Dado = InteiroParaCharBinario(i8086.ULA_OUT, _16BITS);
			}
			insert_string_on_display(Dado, 8 + (i * 3), 28, DISPLAY_WINDOW1);
			free(Dado);
		}
		insert_string_on_display("ULA", 6, 29, DISPLAY_WINDOW1);

#pragma endregion - Vai substituir o local do registrador de segmento...
/*
#pragma region REGISTRADOR SEGMENTO
		insert_string_on_display("SEGMENTO", 6, 29, DISPLAY_WINDOW1);
		for (int i = 0; i < i8086.Registradores_Segmento->Quantidade; i++) {
			create_window_layout(27, 6 + (3*i), 22, 3, false, false, 0, ' ', DISPLAY_WINDOW1); // cria uma "grid"
			insert_string_on_display(i8086.Registradores_Segmento->Registradores[i].Nome, 7+ (3*i), 28, DISPLAY_WINDOW1);
			Dado = InteiroParaCharBinario(i8086.Registradores_Segmento->Registradores[i].Dado, _16BITS);
			insert_string_on_display(Dado, 8 + (i*3), 28, DISPLAY_WINDOW1);
			free(Dado);
		}
#pragma endregion
*/
#pragma region REGISTRADOR FLAGS
		MT2D_Create_Window_Layout(70, 6, 7, i8086.Registrador_Flags->Quantidade+1, false, false, 0, ' ', DISPLAY_WINDOW1); // cria uma "grid"
		insert_string_on_display("FLAGS", 6, 71, DISPLAY_WINDOW1);
		for (int i = 0; i < i8086.Registrador_Flags->Quantidade; i++) {
			insert_string_on_display(i8086.Registrador_Flags->Registradores[i].Nome, 7+i, 71, DISPLAY_WINDOW1);
			insert_number_on_display(i8086.Registrador_Flags->Registradores[i].Dado, 7+i, 74, DISPLAY_WINDOW1); // vai ser sempre 0 ou 1 então não precisa converter pra booleano do tipo string
		}

#pragma endregion
#pragma region REGISTRADOR IP
		
		insert_string_on_display("IP:", 20, 5, DISPLAY_WINDOW1);
		ID = 0;
		Dado = InteiroParaCharBinario(i8086.IP->Registradores[ID].Dado, _16BITS);
		insert_string_on_display(Dado, 20, 9, DISPLAY_WINDOW1);
		free(Dado);
		insert_number_on_display(i8086.IP->Registradores[ID].Dado, 20, 27, DISPLAY_WINDOW1);

#pragma endregion
#pragma region BUS INT
		insert_string_on_display("BUS INT:", 22, 5, DISPLAY_WINDOW1);
		ID = 0;
		Dado = InteiroParaCharBinario(i8086.Bus_Int, _16BITS);
		insert_string_on_display(Dado, 22, 14, DISPLAY_WINDOW1);
		free(Dado);
		insert_number_on_display(i8086.Bus_Int, 22, 32, DISPLAY_WINDOW1);


#pragma endregion
		insert_string_on_display("REGISTRADOR", 0, 10, DISPLAY_WINDOW1);
		MT2D_Draw_Window(DISPLAY_WINDOW1);
		Saida = MT2D_Keyboard_keytouched();
		if (Saida == tab_pressed) {//ta hora de dizer tchau
		}
		else {
			Saida = false;
		}

	} while (!Saida);
}