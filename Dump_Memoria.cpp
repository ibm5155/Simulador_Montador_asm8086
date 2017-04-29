#include "Texto.h"
#include <MT2D/MT2D.h>
#include <MT2D/MT2D_Display.h>
#include <MT2D/MT2D_Layout.h>
#include <MT2D/MT2D_Keyboard.h>
#include "CPU.h"
#include "RAM.h"
#include <stdlib.h>
#include <string.h>


void Dump_Memoria() {
	int pnt = i8086.IP->Registradores[0].Dado;
	char BUFFER[200];
	int i = 0;
	int Y = 2;
	int Saida = false;
	char *Booleano;
	do {
		MT2D_Clear_Main_Window();
		MT2D_Create_Window_Layout(0, 0, MAX_HOR - 1, MAX_VER - 1, false, false, 0, ' ', DISPLAY_WINDOW1);
		insert_string_on_display("Memoria", 0, 10, DISPLAY_WINDOW1);
		for (Y = 0; Y < MAX_VER -2;Y++) {//imprime 24 informações a partir de pnt
			insert_number_on_display(pnt+Y, Y+1, 2, DISPLAY_WINDOW1);
			insert_number_on_display(Memoria[pnt+Y], Y+1, 10, DISPLAY_WINDOW1);
			Booleano = InteiroParaCharBinario(Memoria[pnt + Y], _8BITS);
			insert_string_on_display(Booleano, Y+1, 20, DISPLAY_WINDOW1);
			free(Booleano);
			for (i = i8086.IP->Quantidade - 1; i >= 0 ; i--) {
				if (i8086.IP->Registradores[i].Dado != 0) {
					if (pnt + Y == i8086.IP->Registradores[i].Dado) {
						sprintf(BUFFER, "<--- (%s %d)", (i == 0 ? "IP" : "PILHA"), i);
						insert_string_on_display(BUFFER, Y + 1, 29, DISPLAY_WINDOW1);
					}
				}
			}
		}
		MT2D_Draw_Window(DISPLAY_WINDOW1);
		Saida = MT2D_Keyboard_keytouched();
		if (Saida == tab_pressed) {//ta hora de dizer tchau
		}
		else  if (Saida == arrow_key_pressed) {
			Saida = MT2D_Keyboard_keytouched();
			if (Saida == arrow_up_pressed && pnt > 0) {
				pnt--;
			}
			else if (Saida == arrow_down_pressed && pnt < 65510) {
				pnt++;
			}
			Saida = false;
		}
		else {
			Saida = false;
		}

	} while (!Saida);
}